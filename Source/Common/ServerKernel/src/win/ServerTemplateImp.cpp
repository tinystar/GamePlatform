#include "ServerTemplateImp.h"
#include <process.h>

using namespace std;

#define MAX_MSG_COUNT		0x7FFFFFFF		// LONG_MAX£¬maximum semaphore count


ServerTemplateImp::ServerTemplateImp(ServerTemplate* pApiObj)
	: m_pApiObj(pApiObj)
	, m_hSemaphore(NULL)
	, m_hQuitEvent(NULL)
	, m_hMsgThread(NULL)
	, m_bRunning(false)
{
}

ServerTemplateImp::~ServerTemplateImp()
{
	stop();
}

bool ServerTemplateImp::start()
{
	m_hSemaphore = ::CreateSemaphore(NULL, 0, MAX_MSG_COUNT, NULL);
	if (!EzVerify(m_hSemaphore != NULL))
		return false;

	m_hQuitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!EzVerify(m_hQuitEvent != NULL))
	{
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
		return false;
	}

	m_hMsgThread = (HANDLE)::_beginthreadex(NULL, 0, msgQueueHandleThread, this, 0, NULL);
	if (!EzVerify(m_hMsgThread != NULL))
	{
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
		::CloseHandle(m_hQuitEvent);
		m_hQuitEvent = NULL;
		return false;
	}

	m_bRunning = true;
	return true;
}

bool ServerTemplateImp::stop()
{
	m_bRunning = false;
	if (m_hMsgThread != NULL)
	{
		EzAssert(m_hQuitEvent != NULL);
		EzAssert(m_hSemaphore != NULL);
		::SetEvent(m_hQuitEvent);
		::WaitForSingleObject(m_hMsgThread, INFINITE);
		::CloseHandle(m_hMsgThread);
		m_hMsgThread = NULL;

		::CloseHandle(m_hQuitEvent);
		m_hQuitEvent = NULL;

		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}

	list<QueueItemHead*>::iterator iter = m_msgQueue.begin();
	while (iter != m_msgQueue.end())
	{
		delete *iter;
		++iter;
	}
	m_msgQueue.clear();

	return true;
}

void ServerTemplateImp::onClientConnected(ClientId id)
{
	if (!m_bRunning)
		return;

	QueueClientConnMsg* pClientConn = new QueueClientConnMsg();
	if (!EzVerify(pClientConn != NULL))
	{
		EzLogError(_T("create QueueClientConnMsg failed, maybe memory is not enough!\n"));
		return;
	}

	pClientConn->uItemType = kTypeClientConn;
	pClientConn->clientId = id;

	addQueueItem(pClientConn);
}

void ServerTemplateImp::onPackageReceived(ClientId id, void* pPackage, size_t nSize)
{
	if (!m_bRunning)
		return;

	QueuePackageRecvMsg* pPkgRecv = new QueuePackageRecvMsg();
	if (!EzVerify(pPkgRecv != NULL))
	{
		EzLogError(_T("create QueuePackageRecvMsg failed, maybe memory is not enough!\n"));
		return;
	}

	pPkgRecv->uItemType = kTypePackageRecv;
	pPkgRecv->clientId = id;
	pPkgRecv->uDataSize = (EzUInt32)nSize;
	pPkgRecv->pRecvData = new char[nSize];
	::memcpy(pPkgRecv->pRecvData, pPackage, nSize);

	addQueueItem(pPkgRecv);
}

void ServerTemplateImp::onClientClosed(ClientId id)
{
	if (!m_bRunning)
		return;

	QueueClientCloseMsg* pClientClose = new QueueClientCloseMsg();
	if (!EzVerify(pClientClose != NULL))
	{
		EzLogError(_T("create QueueClientCloseMsg failed, maybe memory is not enough!\n"));
		return;
	}

	pClientClose->uItemType = kTypeClientClose;
	pClientClose->clientId = id;

	addQueueItem(pClientClose);
}

void ServerTemplateImp::onTimerMessage(EzUInt uTimerId)
{
	if (!m_bRunning)
		return;

	QueueTimerMsg* pTimerMsg = new QueueTimerMsg();
	if (!EzVerify(pTimerMsg != NULL))
	{
		EzLogError(_T("create QueueTimerMsg failed, maybe memory is not enough!\n"));
		return;
	}

	pTimerMsg->uItemType = kTypeTimer;
	pTimerMsg->uTimerId = uTimerId;

	addQueueItem(pTimerMsg);
}

bool ServerTemplateImp::addQueueItem(QueueItemHead* pItem)
{
	if (NULL == pItem)
		return false;

	m_queueLock.lock();
	m_msgQueue.push_back(pItem);
	m_queueLock.unlock();
	BOOL bRet = ::ReleaseSemaphore(m_hSemaphore, 1, NULL);
	if (!bRet)
	{
		DWORD dwErr = ::GetLastError();
		EzLogError(_T("ReleaseSemaphore failed, last error code = %d, maybe reached the maximum of the msg queue!\n"), dwErr);
	}

	return !!bRet;
}

unsigned __stdcall ServerTemplateImp::msgQueueHandleThread(void* pParam)
{
	ServerTemplateImp* pServerImp = (ServerTemplateImp*)pParam;
	HANDLE hEvents[2] = { pServerImp->m_hSemaphore, pServerImp->m_hQuitEvent };

	while (true)
	{
		DWORD dwRet = ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
			pServerImp->handleQueueMsg();
		else if ((WAIT_OBJECT_0 + 1) == dwRet)
			break;		// quit
		else
			continue;
	}

	return 0;
}

void ServerTemplateImp::handleQueueMsg()
{
	m_queueLock.lock();
	if (m_msgQueue.empty())
	{
		m_queueLock.unlock();
		return;
	}

	QueueItemHead* pItem = m_msgQueue.front();
	m_msgQueue.pop_front();
	m_queueLock.unlock();

	if (!EzVerify(pItem != NULL))
		return;

	switch (pItem->uItemType)
	{
	case kTypeClientConn:
		{
			QueueClientConnMsg* pClientConn = (QueueClientConnMsg*)pItem;
			m_pApiObj->onTcpClientConnectMsg(pClientConn->clientId);
		}
		break;
	case kTypePackageRecv:
		{
			QueuePackageRecvMsg* pPkgRecv = (QueuePackageRecvMsg*)pItem;
			m_pApiObj->onTcpPackageRecvMsg(pPkgRecv->clientId, pPkgRecv->pRecvData, pPkgRecv->uDataSize);
		}
		break;
	case kTypeClientClose:
		{
			QueueClientCloseMsg* pClientClose = (QueueClientCloseMsg*)pItem;
			m_pApiObj->onTcpClientCloseMsg(pClientClose->clientId);

			m_pApiObj->m_pTcpService->finalReleaseClient(pClientClose->clientId);
		}
		break;
	case kTypeTimer:
		{
			QueueTimerMsg* pTimerMsg = (QueueTimerMsg*)pItem;
			m_pApiObj->onTimerMsg(pTimerMsg->uTimerId);
		}
		break;
	case kTypeUserItem:
		{
			QueueUserItemMsg* pUserItem = (QueueUserItemMsg*)pItem;
			m_pApiObj->onUserItemMsg(pUserItem->itemId, pUserItem->pItemData, pUserItem->uDataSize);
		}
		break;
	default:
		EzAssert(false);
		break;
	}

	delete pItem;
}

bool ServerTemplateImp::queueUserItem(int itemId, void* pData, size_t nSize)
{
	if (!m_bRunning)
		return false;

	QueueUserItemMsg* pUserItem = new QueueUserItemMsg();
	if (!EzVerify(pUserItem != NULL))
	{
		EzLogError(_T("create QueueUserItemMsg failed, maybe memory is not enough!\n"));
		return false;
	}

	pUserItem->uItemType = kTypeUserItem;
	pUserItem->itemId = itemId;
	pUserItem->uDataSize = (EzUInt32)nSize;
	pUserItem->pItemData = new char[nSize];
	::memcpy(pUserItem->pItemData, pData, nSize);

	return addQueueItem(pUserItem);
}

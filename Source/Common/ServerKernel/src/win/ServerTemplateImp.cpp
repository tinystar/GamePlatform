#include "ServerTemplateImp.h"
#include <process.h>

using namespace std;

#define MAX_MSG_COUNT		0x7FFFFFFF		// LONG_MAX£¬maximum semaphore count


ServerTemplateImp::ServerTemplateImp(ServerTemplate* pApiObj)
	: m_pApiObj(pApiObj)
	, m_hSemaphore(NULL)
	, m_hQuitEvent(NULL)
	, m_hMsgThread(NULL)
{
}

ServerTemplateImp::~ServerTemplateImp()
{
	unInit();
}

bool ServerTemplateImp::init()
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

	return true;
}

bool ServerTemplateImp::unInit()
{
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
	cleanQueueItemAbout(id);

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
		}
		break;
	case kTypeTimer:
		{
			QueueTimerMsg* pTimerMsg = (QueueTimerMsg*)pItem;
			m_pApiObj->onTimerMsg(pTimerMsg->uTimerId);
		}
		break;
	default:
		EzAssert(false);
		break;
	}

	delete pItem;
}

// the ClientContext will be reused immediately after it was closed, but the queue
// may remain some items associated with the ClientContext, if these item continue to
// be processed, that will cause problems. So we should clean these items when associated
// ClientContext was closed.Another possible solution is to use reference count to manage
// the life cycle of the ClientContext.
void ServerTemplateImp::cleanQueueItemAbout(ClientId id)
{
	m_queueLock.lock();
	list<QueueItemHead*>::iterator iter = m_msgQueue.begin();
	while (iter != m_msgQueue.end())
	{
		bool bNeedErase = false;

		QueueItemHead* pItem = *iter;
		if (kTypeClientConn == pItem->uItemType)
		{
			QueueClientConnMsg* pClientConn = (QueueClientConnMsg*)pItem;
			if (id == pClientConn->clientId)
				bNeedErase = true;
		}
		else if (kTypePackageRecv == pItem->uItemType)
		{
			QueuePackageRecvMsg* pPkgRecv = (QueuePackageRecvMsg*)pItem;
			if (id == pPkgRecv->clientId)
				bNeedErase = true;
		}

		if (bNeedErase)
		{
			delete pItem;
			iter = m_msgQueue.erase(iter);
			continue;
		}

		++iter;
	}
	m_queueLock.unlock();
}

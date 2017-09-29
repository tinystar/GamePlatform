#include "ClientContext.h"

#define DEFAULT_PACKAGE_SIZE	1024
#define SEND_WAIT_TIMEOUT		(3*60*1000)			// 3 minutes


bool ClientContext::init(IClientSessionMgr* pManager, size_t nMaxPkgSize)
{
	if (kNewCreated == m_status)
	{
		if (!EzVerify(pManager != NULL))
			return false;

		m_pClientMgr = pManager;

		if (!EzVerify(nMaxPkgSize != 0))
			m_nMaxPkgSize = DEFAULT_PACKAGE_SIZE;
		else
			m_nMaxPkgSize = nMaxPkgSize;

		m_sendBuffer.init(m_nMaxPkgSize);
		m_recvBuffer.init(m_nMaxPkgSize);

		m_status = kInitialized;
	}

	return true;
}

bool ClientContext::asyncRecv()
{
	EzAutoLocker autoLocker(&m_recvLock);

	if (m_status != kConnected && m_status != kClosing)
		return false;

	return m_recvBuffer.asyncRecv(this, &m_recvIoContext, m_pClientMgr);
}

bool ClientContext::asyncSend(void* pData, size_t nDataLen)
{
	if (m_status != kConnected)
		return false;

	if (m_sendBuffer.getFreeSize() < nDataLen)
	{
		m_bWaitSend = true;
		m_nWaitSendSize = nDataLen;
		DWORD dwRet = ::WaitForSingleObject(m_hWaitSendEvent, SEND_WAIT_TIMEOUT);
		if (dwRet != WAIT_OBJECT_0)
		{
			EzLogWarn(_T("asyncSend wait failure!!! return value = %d\n"), dwRet);
			return false;
		}
	}

	EzAutoLocker autoLocker(&m_sendLock);

	if (m_status != kConnected)
		return false;

	return m_sendBuffer.asyncSend(this, &m_sendIoContext, m_pClientMgr, pData, nDataLen);
}

bool ClientContext::onRecvCompleted()
{
	EzAutoLocker autoLocker(&m_recvLock);

	if (m_status != kConnected && m_status != kClosing)
		return false;

	if (m_recvBuffer.onRecvCompleted(this, m_pClientMgr))
		return asyncRecv();

	return false;
}

bool ClientContext::onSendCompleted(size_t nSize)
{
	EzAutoLocker autoLocker(&m_sendLock);

	if (m_status != kConnected && m_status != kClosing)
		return false;

	bool bRet = m_sendBuffer.onSendCompleted(this, &m_sendIoContext, nSize, m_pClientMgr);

	if (m_bWaitSend)
	{
		if (m_sendBuffer.getFreeSize() >= m_nWaitSendSize)
		{
			::SetEvent(m_hWaitSendEvent);
			m_bWaitSend = false;
			m_nWaitSendSize = 0;
		}
	}

	return bRet;
}

bool ClientContext::closeClient(bool bGraceful)
{
	m_closeLock.lock();
	if (m_status != kConnected)
	{
		m_closeLock.unlock();
		return false;
	}

	m_status = kClosing;
	m_closeLock.unlock();

	int nWaitCount = 100;
	if (m_bWaitSend)
	{
		::SetEvent(m_hWaitSendEvent);
		m_bWaitSend = false;
		m_nWaitSendSize = 0;
		nWaitCount = 20;
		while (nWaitCount-- != 0)	// wait for a few moment
			::Sleep(0);
	}

	if (bGraceful)
	{
		// Todo: 超时及发送错误检测
		nWaitCount = 100;
		while (!m_sendBuffer.isEmpty() && nWaitCount-- > 0)
			::Sleep(0);		// waiting to send complete.
	}

	// According to MSDN:  A Winsock client must never issue closesocket on s concurrently with another Winsock function call.
	EzAutoLocker sendLocker(&m_sendLock);
	EzAutoLocker recvLocker(&m_recvLock);

	if (!EzVerify(m_hSocket != INVALID_SOCKET))
		return false;

	if (!bGraceful)
	{
		//
		// force the subsequent closesocket to be abortive.
		//
		LINGER lingerStruct;

		lingerStruct.l_onoff = 1;
		lingerStruct.l_linger = 0;
		::setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));
	}

	::closesocket(m_hSocket);
	m_hSocket = INVALID_SOCKET;

	::memset(&m_address, 0, sizeof(m_address));

	// 等待send、recv的异步io完成
	nWaitCount = 50;
	while (!HasOverlappedIoCompleted(&m_sendIoContext.overlapped) && nWaitCount-- > 0)
		::Sleep(0);
	nWaitCount = 50;
	while (!HasOverlappedIoCompleted(&m_recvIoContext.overlapped) && nWaitCount-- > 0)
		::Sleep(0);

	m_sendIoContext.reset();
	m_recvIoContext.reset();

	m_sendBuffer.reset();
	m_recvBuffer.reset();

	m_status = kClosed;

	m_pUserData = NULL;

	return true;
}

void ClientContext::dump(EzDumpContext& dc) const
{
	dc << "------------start dump ClientContext------------\n";
	dc << "this: " << this << "\n";
	dc << "socket: " << m_hSocket << "\n";
	dc << "status: " << Status2String(m_status) << "\n";
	dc << "current send size: " << m_sendBuffer.getUsedSize() << "\n";
	dc << "current recv size: " << m_recvBuffer.getUsedSize() << "\n";
	dc << "------------ClientContext dump over!------------\n";
}

const char* ClientContext::Status2String(Status status) const
{
	const char* pszStatus = "unknown status";

	switch (status)
	{
	case kNewCreated:
		pszStatus = "NewCreated";
		break;
	case kInitialized:
		pszStatus = "Initialized";
		break;
	case kConnected:
		pszStatus = "Connected";
		break;
	case kClosing:
		pszStatus = "Closing";
		break;
	case kClosed:
		pszStatus = "Closed";
		break;
	}
	return pszStatus;
}

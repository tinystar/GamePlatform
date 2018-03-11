#include "TcpServiceIocp.h"
#include <process.h>
#include "ClientContext.h"
#include <mswsock.h>
#include "ServiceExcp.h"

using namespace EzSys;
using namespace std;

#define GRACEFUL_CLOSE_SIGNAL			((DWORD)-1)
#define FORCIBLE_CLOSE_SIGNAL			((DWORD)-2)

struct WorkerThreadParam
{
	TcpServiceIocp* pIocpService;
	HANDLE			hIocp;
	HANDLE			hEvent;
};

struct SendToAllThreadParam
{
	TcpServiceIocp* pIocpService;
	HANDLE			hEvent;
};

BOOL PASCAL FAR MsAcceptEx(SOCKET sListen, 
						   SOCKET sAccept, 
						   PVOID lpOutBuffer, 
						   DWORD dwRecvLen, 
						   DWORD dwLocalAddrLen, 
						   DWORD dwRemoteAddrLen,
						   LPDWORD lpdwBytesReceived,
						   LPOVERLAPPED lpOverlapped)
{
	static LPFN_ACCEPTEX s_lpfnAcceptEx = NULL;
	static EzLock s_lock;

	if (NULL == s_lpfnAcceptEx)
	{
		EzAutoLocker autoLock(&s_lock);
		if (NULL == s_lpfnAcceptEx)
		{
			SOCKET hTmpSock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			DWORD dwBytes = 0;
			GUID guidAcceptEx = WSAID_ACCEPTEX;
			int iResult = ::WSAIoctl(hTmpSock,
									 SIO_GET_EXTENSION_FUNCTION_POINTER,
									 &guidAcceptEx, sizeof(guidAcceptEx),
									 &s_lpfnAcceptEx, sizeof(s_lpfnAcceptEx),
									 &dwBytes, NULL, NULL);
			::closesocket(hTmpSock);
			if (!EzVerify(0 == iResult))
				EzLogError(_T("obtain AcceptEx function pointer failed! error code = %d\n"), ::WSAGetLastError());
		}
	}
	if (!EzVerify(s_lpfnAcceptEx != NULL))
	{
		throw ServiceExcp(_T("obtain AcceptEx function failure!"), eSystemError, _T(__FILE__), __LINE__);
		return FALSE;
	}

	return s_lpfnAcceptEx(sListen, sAccept, lpOutBuffer, dwRecvLen, dwLocalAddrLen, dwRemoteAddrLen, lpdwBytesReceived, lpOverlapped);
}

void PASCAL FAR MsGetAcceptExSockaddrs(PVOID lpOutputBuffer,
									   DWORD dwReceiveDataLength,
									   DWORD dwLocalAddressLength,
									   DWORD dwRemoteAddressLength,
									   struct sockaddr **LocalSockaddr,
									   LPINT LocalSockaddrLength,
									   struct sockaddr **RemoteSockaddr,
									   LPINT RemoteSockaddrLength)
{
	static LPFN_GETACCEPTEXSOCKADDRS s_lpfnGetAcceptExSockaddrs = NULL;
	static EzLock s_lock;

	if (NULL == s_lpfnGetAcceptExSockaddrs)
	{
		EzAutoLocker autoLock(&s_lock);
		if (NULL == s_lpfnGetAcceptExSockaddrs)
		{
			SOCKET hTmpSock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			DWORD dwBytes = 0;
			GUID guidGetAExSA = WSAID_GETACCEPTEXSOCKADDRS;
			int iResult = ::WSAIoctl(hTmpSock,
									 SIO_GET_EXTENSION_FUNCTION_POINTER,
									 &guidGetAExSA, sizeof(guidGetAExSA),
									 &s_lpfnGetAcceptExSockaddrs, sizeof(s_lpfnGetAcceptExSockaddrs),
									 &dwBytes, NULL, NULL);
			::closesocket(hTmpSock);
			if (!EzVerify(0 == iResult))
				EzLogError(_T("obtain GetAcceptExSockaddrs function pointer failed! error code = %d\n"), ::WSAGetLastError());
		}
	}
	if (!EzVerify(s_lpfnGetAcceptExSockaddrs != NULL))
	{
		throw ServiceExcp(_T("obtain GetAcceptExSockaddrs function failure!"), eSystemError, _T(__FILE__), __LINE__);
		return;
	}

	s_lpfnGetAcceptExSockaddrs(lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength,
								LocalSockaddr, LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength);
}


//-------------------------------------------------------------------------------
// TcpServiceIocp
//-------------------------------------------------------------------------------
TcpServiceIocp::TcpServiceIocp()
	: m_ulAddress(0)
	, m_usPort(0)
	, m_listenSock(INVALID_SOCKET)
	, m_hIocp(INVALID_HANDLE_VALUE)
	, m_nThreadCount(0)
	, m_hThreadEvent(INVALID_HANDLE_VALUE)
	, m_nAcceptCount(0)
	, m_nPackageSize(0)
	, m_bInitialized(false)
	, m_bRunning(false)
	, m_hSendToAllThread(NULL)
	, m_hSendCacheSem(NULL)
	, m_hSendQuitEvent(NULL)
	, m_handleSeed(0)
{
}

TcpServiceIocp::~TcpServiceIocp()
{
	stop();
	unInit();
}

SVCErrorCode TcpServiceIocp::initFromConfig(const TcpConfig& config)
{
	if (0 == config.szAddr[0])
		m_ulAddress = INADDR_ANY;
	else
		m_ulAddress = inet_addr(config.szAddr);

	m_usPort = config.sPort;
	
	if (0 == config.nSockThreadCnt)
		m_nThreadCount = 2 * getProcessorCount();
	else
		m_nThreadCount = config.nSockThreadCnt;

	if (0 == config.nMaxAcceptCnt)
		m_nAcceptCount = 10;
	else
		m_nAcceptCount = config.nMaxAcceptCnt;

	if (0 == config.nMaxPackageSize)
		m_nPackageSize = 1024;
	else
		m_nPackageSize = config.nMaxPackageSize;

	return eOk;
}

SVCErrorCode TcpServiceIocp::init(const TcpConfig& config /*= TcpConfig::kDefault*/)
{
	if (m_bInitialized)
		return eOk;

	SVCErrorCode errCode = eOk;
	errCode = initFromConfig(config);
	if (!EzVerify(eOk == errCode))
		return errCode;

	m_bInitialized = true;
	return eOk;
}

SVCErrorCode TcpServiceIocp::unInit()
{
	if (!m_bInitialized)
		return eOk;

	m_ulAddress = 0;
	m_usPort = 0;
	m_nThreadCount = 0;
	m_nAcceptCount = 0;
	m_nPackageSize = 0;

	// delete all the client context node
	m_ClientContextMgr.releaseClosedArray();
	m_ClientContextMgr.clearFreeClientList();

	m_bInitialized = false;
	return eOk;
}

SVCErrorCode TcpServiceIocp::start()
{
	if (!m_bInitialized)
		return eNotInitialized;
	if (m_bRunning)
		return eOk;

	SVCErrorCode errCode = eOk;
	errCode = initListenSock();
	if (!EzVerify(eOk == errCode))
		return errCode;

	errCode = initIocp();
	if (!EzVerify(eOk == errCode))
	{
		::closesocket(m_listenSock);
		m_listenSock = INVALID_SOCKET;
		return errCode;
	}

	HANDLE hExistIocp = ::CreateIoCompletionPort((HANDLE)m_listenSock, m_hIocp, 0, 0);
	if (!EzVerify(hExistIocp == m_hIocp))
	{
		::closesocket(m_listenSock);
		m_listenSock = INVALID_SOCKET;
		::CloseHandle(m_hIocp);
		m_hIocp = INVALID_HANDLE_VALUE;
		return eSystemError;
	}

	m_hThreadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == m_hThreadEvent)
	{
		::closesocket(m_listenSock);
		m_listenSock = INVALID_SOCKET;
		::CloseHandle(m_hIocp);
		m_hIocp = INVALID_HANDLE_VALUE;
		return eSystemError;
	}

	HANDLE hThread = INVALID_HANDLE_VALUE;
	WorkerThreadParam threadParam;
	threadParam.pIocpService = this;
	threadParam.hIocp = m_hIocp;
	threadParam.hEvent = m_hThreadEvent;
	unsigned int nFailThreadCount = 0;

	for (unsigned int i = 0; i < m_nThreadCount; ++i)
	{
		hThread = (HANDLE)::_beginthreadex(NULL, 0, iocpWorkerThread, &threadParam, 0, NULL);
		if (!EzVerify(hThread != NULL))
		{
			EzLogWarn(_T("create iocp worker thread failure! error code = %d\n"), ::GetLastError());
			++nFailThreadCount;
			continue;
		}
		::WaitForSingleObject(m_hThreadEvent, INFINITE);
		::ResetEvent(m_hThreadEvent);
		::CloseHandle(hThread);
	}
	m_nThreadCount -= nFailThreadCount;

	for (unsigned int i = 0; i < m_nAcceptCount; ++i)
	{
		PER_ACCEPT_CONTEXT* pAcceptContext = new PER_ACCEPT_CONTEXT;
		if (EzVerify(postAccept(pAcceptContext)))
			m_AcceptContexts.push_back(pAcceptContext);
		else
			delete pAcceptContext;
	}

	m_bRunning = true;
	return eOk;
}

SVCErrorCode TcpServiceIocp::stop()
{
	if (!m_bRunning)
		return eOk;

	stopSendToAllThread();

	closeAllClient();

	for (unsigned int i = 0; i < m_nThreadCount; ++i)
	{
		::PostQueuedCompletionStatus(m_hIocp, 0, 0, NULL);
		::WaitForSingleObject(m_hThreadEvent, INFINITE);
		::ResetEvent(m_hThreadEvent);
	}

	::CloseHandle(m_hThreadEvent);
	m_hThreadEvent = INVALID_HANDLE_VALUE;

	for (size_t i = 0; i < m_AcceptContexts.size(); ++i)
	{
		delete m_AcceptContexts[i];
	}
	m_AcceptContexts.clear();

	::closesocket(m_listenSock);
	m_listenSock = INVALID_SOCKET;

	::CloseHandle(m_hIocp);
	m_hIocp = INVALID_HANDLE_VALUE;

	m_bRunning = false;
	return eOk;
}

SVCErrorCode TcpServiceIocp::initListenSock()
{
	m_listenSock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (!EzVerify(m_listenSock != INVALID_SOCKET))
	{
		EzLogError(_T("create listen socket failure! error code = %d\n"), ::WSAGetLastError());
		return eCreateSockFail;
	}

	sockaddr_in serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(m_ulAddress);
	serverAddr.sin_port = htons(m_usPort);

	int err = ::bind(m_listenSock, (struct sockaddr*)&serverAddr, sizeof(sockaddr_in));
	if (!EzVerify(err == 0))
	{
		EzLogError(_T("bind listen socket failure! error code = %d\n"), ::WSAGetLastError());
		return eBindSockFail;
	}

	err = ::listen(m_listenSock, SOMAXCONN);
	if (!EzVerify(err == 0))
	{
		EzLogError(_T("listen failure! error code = %d\n"), ::WSAGetLastError());
		return eListenFail;
	}

	return eOk;
}

SVCErrorCode TcpServiceIocp::initIocp()
{
	m_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!EzVerify(m_hIocp != NULL))
		return eSystemError;

	return eOk;
}

bool TcpServiceIocp::sendData(ClientId id, void* pData, size_t nDataLen)
{
	if (id.isNull())
		return false;
	if (NULL == pData || nDataLen <= 0)
		return false;

	ClientContext* pClient = id;
	return pClient->asyncSend(pData, nDataLen);
}

bool TcpServiceIocp::closeClient(ClientId id, bool bGraceful /*= true*/)
{
	if (id.isNull())
		return false;

	notifyCloseClient((ClientContext*)id, bGraceful);
	return true;
}

bool TcpServiceIocp::closeAndFreeClient(ClientContext* pClient, bool bGraceful)
{
	if (NULL == pClient)
		return false;

	if (pClient->closeClient(bGraceful))
	{
		for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
		{
			ITcpServiceEventHandler* pEventHandler = m_EventHandlers[i];
			if (pEventHandler)
				pEventHandler->onClientClosed(pClient);
		}

		m_ClientContextMgr.closeClientContext(pClient);
	}

	return true;
}

void TcpServiceIocp::closeAllClient()
{
	ClientCtxIterator* pIter = m_ClientContextMgr.newIterator();
	if (pIter)
	{
		while (!pIter->isDone())
		{
			ClientContext* pCurClient = pIter->getClientCtx();
			notifyCloseClient(pCurClient, true);

			pIter->step();
		}
		delete pIter;
	}
}

void TcpServiceIocp::notifyCloseClient(ClientContext* pClient, bool bGraceful)
{
	if (NULL == pClient)
		return;

	::PostQueuedCompletionStatus(m_hIocp, 
								 bGraceful ? GRACEFUL_CLOSE_SIGNAL : FORCIBLE_CLOSE_SIGNAL, 
								 (ULONG_PTR)pClient, 
								 NULL);
}

bool TcpServiceIocp::postAccept(PER_ACCEPT_CONTEXT* pAcceptContext)
{
	if (!EzVerify(pAcceptContext != NULL))
		return false;
	if (!EzVerify(m_listenSock != INVALID_SOCKET))
		return false;

	// create a socket to prepare the new client to connect.
	pAcceptContext->hAcceptSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (!EzVerify(pAcceptContext->hAcceptSocket != INVALID_SOCKET))
		return false;

	DWORD dwBytes = 0;
	if (!MsAcceptEx(m_listenSock, pAcceptContext->hAcceptSocket, pAcceptContext->szAcceptBuffer, 0,
					sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &pAcceptContext->overlapped))
	{
		int iErrCode = ::WSAGetLastError();
		if (iErrCode != WSA_IO_PENDING)
		{
			EzLogError(_T("AcceptEx failure! error code = %d\n"), iErrCode);
			return false;
		}
	}

	return true;
}

bool TcpServiceIocp::doAccept(PER_ACCEPT_CONTEXT* pAcceptContext, DWORD dwBytes)
{
	class _auto_realease
	{
	public:
		_auto_realease(TcpServiceIocp* pService, PER_ACCEPT_CONTEXT* pAcceptCtx)
			: m_pIocpService(pService)
			, m_pAcceptContext(pAcceptCtx)
		{
		}
		~_auto_realease()
		{
			if (m_pIocpService && m_pAcceptContext)
			{
				// continue to post accept request on PER_ACCEPT_CONTEXT.
				m_pAcceptContext->reset();
				if (!EzVerify(m_pIocpService->postAccept(m_pAcceptContext)))
				{
					EzLogError(_T("continue to post accept fails!\n"));
				}
			}
		}

	private:
		TcpServiceIocp*		m_pIocpService;
		PER_ACCEPT_CONTEXT*	m_pAcceptContext;
	};

	if (!EzVerify(pAcceptContext != NULL))
		return false;

	_auto_realease _autoObj(this, pAcceptContext);

	int iResult = ::setsockopt(pAcceptContext->hAcceptSocket,
							   SOL_SOCKET,
							   SO_UPDATE_ACCEPT_CONTEXT,
							   (char *)&m_listenSock,
							   sizeof(m_listenSock));
	if (SOCKET_ERROR == iResult)
	{
		EzAssert(false);
		EzLogError(_T("failed to update accept socket\n"));
	}
	// set socket to non-blocking mode
	u_long ulMode = 1;
	iResult = ::ioctlsocket(pAcceptContext->hAcceptSocket, FIONBIO, &ulMode);
	if (iResult < 0)
	{
		EzAssert(false);
		EzLogError(_T("failed to set socket to non-blocking mode.\n"));
	}

	SOCKADDR_IN* pClientAddr = NULL;
	SOCKADDR_IN* pLocalAddr = NULL;
	int iClientLen = sizeof(SOCKADDR_IN);
	int iLocalLen = sizeof(SOCKADDR_IN);

	MsGetAcceptExSockaddrs(pAcceptContext->szAcceptBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
						   (sockaddr**)&pLocalAddr, &iLocalLen, (sockaddr**)&pClientAddr, &iClientLen);

	if (m_pClientFilter &&
		!m_pClientFilter->filterNewClient(pAcceptContext->hAcceptSocket, pClientAddr))
	{
		// refuse the client
		EzLogInfo(_T("refuse the socket(%d)\n"), pAcceptContext->hAcceptSocket);		
		::closesocket(pAcceptContext->hAcceptSocket);		// must be closed.
		return false;
	}

	ClientContext* pClientCtx = m_ClientContextMgr.newClientContext();
	if (!EzVerify(pClientCtx))
	{
		::closesocket(pAcceptContext->hAcceptSocket);		// must be closed.
		return false;
	}

	bool bRet = pClientCtx->init(this, m_nPackageSize);
	if (!EzVerify(bRet))
	{
		::closesocket(pAcceptContext->hAcceptSocket);		// must be closed.
		m_ClientContextMgr.closeAndFreeClientContext(pClientCtx);
		return false;
	}

	bRet = pClientCtx->attach(pAcceptContext->hAcceptSocket, *pClientAddr, generateUniqueHandle());
	if (!EzVerify(bRet))
	{
		::closesocket(pAcceptContext->hAcceptSocket);		// must be closed.
		m_ClientContextMgr.closeAndFreeClientContext(pClientCtx);
		return false;
	}

	HANDLE hExistIocp = ::CreateIoCompletionPort((HANDLE)pClientCtx->getSocket(), m_hIocp, (ULONG_PTR)pClientCtx, 0);
	if (!EzVerify(hExistIocp != NULL))
	{
		EzLogError(_T("associate with iocp fails, error code = %d\n"), ::GetLastError());
		closeAndFreeClient(pClientCtx, false);
		return false;
	}
	
	// notify new client connected.
	// after associated with iocp otherwise sendData in onClientConnected callback function will have no notice in iocp.
	for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
	{
		ITcpServiceEventHandler* pEventHandler = m_EventHandlers[i];
		if (pEventHandler)
			pEventHandler->onClientConnected(pClientCtx);
	}

	bRet = pClientCtx->asyncRecv();
	if (!EzVerify(bRet))
	{
		EzLogError(_T("client post recv fails!\n"));
		closeAndFreeClient(pClientCtx, false);
		return false;
	}

	return true;
}

unsigned __stdcall TcpServiceIocp::iocpWorkerThread(void* pParam)
{
	WorkerThreadParam* pThreadParam = (WorkerThreadParam*)pParam;
	TcpServiceIocp* pTcpService = pThreadParam->pIocpService;
	HANDLE hIocp = pThreadParam->hIocp;
	HANDLE hThreadEvent = pThreadParam->hEvent;

	::SetEvent(hThreadEvent);

	DWORD dwBytesTransferred = 0;
	ULONG_PTR completionKey = 0;
	OVERLAPPED* pOverlapped = NULL;

	while (true)
	{
		BOOL bRet = ::GetQueuedCompletionStatus(hIocp, &dwBytesTransferred, &completionKey, &pOverlapped, INFINITE);
		if (!bRet)
		{
			DWORD dwErr = ::GetLastError();
			if (ERROR_NETNAME_DELETED == dwErr)			// maybe remote socket is closed.
			{
				EzLogInfo(_T("GetQueuedCompletionStatus fails, error is ERROR_NETNAME_DELETED\n"));
				EzAssert(completionKey != 0);
				pTcpService->closeAndFreeClient((ClientContext*)completionKey, false);
				continue;
			}
			else if (ERROR_OPERATION_ABORTED == dwErr)	// maybe this socket is closed.
			{
				EzLogInfo(_T("GetQueuedCompletionStatus fails, error is ERROR_OPERATION_ABORTED\n"));
				continue;
			}
			else
			{
				EzLogWarn(_T("GetQueuedCompletionStatus returns false. error code = %d\n"), dwErr);
			}

			continue;
		}
		// tcp service have been stopped.
		if (0 == completionKey && NULL == pOverlapped)
		{
			::SetEvent(hThreadEvent);
			break;
		}
		if (dwBytesTransferred == GRACEFUL_CLOSE_SIGNAL ||
			dwBytesTransferred == FORCIBLE_CLOSE_SIGNAL)
		{
			bool bGraceful = dwBytesTransferred == GRACEFUL_CLOSE_SIGNAL;
			pTcpService->closeAndFreeClient((ClientContext*)completionKey, bGraceful);
			continue;
		}

		if (!EzVerify(pOverlapped != NULL))
		{
			throw ServiceExcp(_T("NULL OVERLAPPED pointer in iocp working thread!!"), eNullPointer, _T(__FILE__), __LINE__);
			break;
		}

		PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, overlapped);
		EzAssert(pIoContext != NULL);

		// client closed.
		if (0 == dwBytesTransferred && (eOpSend == pIoContext->type))
		{
			EzLogInfo(_T("dwBytesTransferred is 0, the socket will be closed.\n"));
			EzAssert(completionKey != 0);
			pTcpService->closeAndFreeClient((ClientContext*)completionKey, false);
			continue;
		}

		switch (pIoContext->type)
		{
		case eOpAccept:
			{
				if (!EzVerify(0 == completionKey))
					EzLogInfo(_T("completion key should be zero if AcceptEx request completed.\n"));

				pTcpService->doAccept((PER_ACCEPT_CONTEXT*)pIoContext, dwBytesTransferred);
			}
			break;
		case eOpRecv:
			{
				ClientContext* pClientCtx = (ClientContext*)completionKey;
				if (!EzVerify(pClientCtx != NULL))
				{
					throw ServiceExcp(_T("ClientContext is NULL when recv completed."), eNullPointer, _T(__FILE__), __LINE__);
					break;
				}

				pClientCtx->onRecvCompleted();
			}
			break;
		case eOpSend:
			{
				ClientContext* pClientCtx = (ClientContext*)completionKey;
				if (!EzVerify(pClientCtx != NULL))
				{
					throw ServiceExcp(_T("ClientContext is NULL when send completed."), eNullPointer, _T(__FILE__), __LINE__);
					break;
				}

				pClientCtx->onSendCompleted(dwBytesTransferred);
			}
			break;
		default:
			EzAssert(false);
			break;
		}
	}
	 
	return 0;
}

bool TcpServiceIocp::OnErrorOccurredNotify(ClientContext* pClient, SVCErrorCode errType, int iErrCode)
{
	EzLogError(_T("OnErrorOccurredNotify, client = %x, SVCErrorCode = %d\n"), pClient, errType);

	closeAndFreeClient(pClient, false);
	return true;
}

bool TcpServiceIocp::OnPackageRecvedNotify(ClientContext* pClient, void* pPackage, size_t nSize)
{
	if (!EzVerify(pClient != NULL && pPackage != NULL))
		return false;

	for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
	{
		ITcpServiceEventHandler* pEventHandler = m_EventHandlers[i];
		if (pEventHandler)
			pEventHandler->onPackageReceived(pClient, pPackage, nSize);
	}

	return false;
}

void TcpServiceIocp::dump(EzDumpContext& dc) const
{
	dc << "------------start dump TcpServiceIocp------------\n";
	dc << "current active client count: " << m_ClientContextMgr.getClientCount() << "\n";
	dc << "free client node count: " << m_ClientContextMgr.getFreeCount() << "\n";
	dc << "------------TcpServiceIocp dump over!------------\n";
}

size_t TcpServiceIocp::getClientCount() const
{
	return m_ClientContextMgr.getClientCount();
}

ClientIdIterator* TcpServiceIocp::newIterator() const
{
	return m_ClientContextMgr.newIterator();
}

bool TcpServiceIocp::sendDataToAll(void* pData, size_t nDataLen)
{
	if (NULL == pData || nDataLen <= 0 || nDataLen > m_nPackageSize)
		return false;

	if (NULL == m_hSendToAllThread)
	{
		if (!EzVerify(initSendToAllThread()))
			return false;
	}

	SendDataCache data;
	data.pData = new char[nDataLen];
	if (NULL == data.pData)
		return false;

	::memcpy(data.pData, pData, nDataLen);
	data.nDataLen = nDataLen;

	m_cacheListLock.lock();
	m_sendDataCache.push_back(data);
	m_cacheListLock.unlock();

	BOOL bRet = ::ReleaseSemaphore(m_hSendCacheSem, 1, NULL);
	if (!bRet)
	{
		DWORD dwErr = ::GetLastError();
		EzLogError(_T("Release send to all semaphore failed, error code = %d!\n"), dwErr);
	}

	return true;
}

bool TcpServiceIocp::initSendToAllThread()
{
	HANDLE hThreadEvt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == hThreadEvt)
		return false;

	m_hSendCacheSem = ::CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	if (NULL == m_hSendCacheSem)
	{
		::CloseHandle(hThreadEvt);
		return false;
	}

	m_hSendQuitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hSendQuitEvent)
	{
		::CloseHandle(hThreadEvt);
		::CloseHandle(m_hSendCacheSem);
		m_hSendCacheSem = NULL;
		return false;
	}

	SendToAllThreadParam threadParam;
	threadParam.pIocpService = this;
	threadParam.hEvent = hThreadEvt;
	m_hSendToAllThread = (HANDLE)::_beginthreadex(NULL, 0, sendToAllThread, &threadParam, 0, NULL);
	if (NULL == m_hSendToAllThread)
	{
		::CloseHandle(hThreadEvt);
		::CloseHandle(m_hSendCacheSem);
		m_hSendCacheSem = NULL;
		::CloseHandle(m_hSendQuitEvent);
		m_hSendQuitEvent = NULL;
		return false;
	}

	::WaitForSingleObject(hThreadEvt, INFINITE);
	::CloseHandle(hThreadEvt);
	return true;
}

void TcpServiceIocp::stopSendToAllThread()
{
	if (NULL == m_hSendToAllThread)
		return;

	::SetEvent(m_hSendQuitEvent);
	::WaitForSingleObject(m_hSendToAllThread, INFINITE);
	::CloseHandle(m_hSendToAllThread);
	m_hSendToAllThread = NULL;

	::CloseHandle(m_hSendQuitEvent);
	m_hSendQuitEvent = NULL;

	::CloseHandle(m_hSendCacheSem);
	m_hSendCacheSem = NULL;

	SendCacheList::iterator iter = m_sendDataCache.begin();
	while (iter != m_sendDataCache.end())
	{
		delete[] iter->pData;
		++iter;
	}
	m_sendDataCache.clear();
}

void TcpServiceIocp::sendToAllProc()
{
	m_cacheListLock.lock();
	EzAssert(m_sendDataCache.size() > 0);
	SendDataCache data = m_sendDataCache.front();
	m_sendDataCache.pop_front();
	m_cacheListLock.unlock();

	ClientCtxIterator* pIter = m_ClientContextMgr.newIterator();
	if (pIter)
	{
		while (!pIter->isDone())
		{
			sendData(pIter->getClientId(), data.pData, data.nDataLen);

			pIter->step();
		}
		delete pIter;
	}

	delete[] data.pData;
}

unsigned __stdcall TcpServiceIocp::sendToAllThread(void* pParam)
{
	SendToAllThreadParam* pThreadParam = (SendToAllThreadParam*)pParam;
	TcpServiceIocp* pTcpService = pThreadParam->pIocpService;
	HANDLE hThreadEvent = pThreadParam->hEvent;

	::SetEvent(hThreadEvent);		// init success

	HANDLE hObjects[2] = { pTcpService->m_hSendCacheSem, pTcpService->m_hSendQuitEvent };

	while (true)
	{
		DWORD dwRet = ::WaitForMultipleObjects(2, hObjects, FALSE, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
			pTcpService->sendToAllProc();
		else if ((WAIT_OBJECT_0 + 1) == dwRet)
			break;		// quit
		else
			continue;
	}

	return 0;
}

void TcpServiceIocp::finalReleaseClient(ClientId id)
{
	if (id.isNull())
		return;

	ClientContext* pClient = (ClientContext*)id;
	pClient->reset();
	m_ClientContextMgr.freeClientContext(pClient);
}

CSHANDLE TcpServiceIocp::generateUniqueHandle()
{
	return ++m_handleSeed;
}

#include "TcpClientSocketImp.h"
#include "TcpPackage.h"


TcpClientSocketImp::SocketToClientMap TcpClientSocketImp::ms_SocketsMap;
EzLock TcpClientSocketImp::ms_mapLock;

TcpClientSocketImp::TcpClientSocketImp(TcpClientSocket* pApiObj, size_t nMaxPkgSize)
	: m_pApiObj(pApiObj)
	, m_nMaxPkgSize(nMaxPkgSize)
	, m_hSocket(INVALID_SOCKET)
	, m_pSendBuffer(NULL)
	, m_nSendBufSize(0)
	, m_nSendBufUsed(0)
	, m_pRecvBuffer(NULL)
	, m_nRecvBufSize(0)
	, m_nRecvBufUsed(0)
	, m_sockStatus(TcpClientSocket::kInvalid)
	, m_bBlocking(true)
	, m_bWaitSend(false)
{
	EzAssert(m_pApiObj != NULL);
}

TcpClientSocketImp::~TcpClientSocketImp()
{
	close();
}

void TcpClientSocketImp::select(unsigned int nTimeOut)
{
	fd_set fdRead;
	fd_set fdWrite;
	
	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);

	ms_mapLock.lock();
	if (ms_SocketsMap.empty())
	{
		ms_mapLock.unlock();
		return;
	}

	EzArray<SOCKET> selSocks(ms_SocketsMap.size());
	SocketToClientMap::iterator iter = ms_SocketsMap.begin();
	while (iter != ms_SocketsMap.end())
	{
		selSocks.append(iter->first);
		FD_SET(iter->first, &fdRead);
		TcpClientSocketImp* pClientImp = iter->second;
		if (pClientImp->m_sockStatus == TcpClientSocket::kConnecting ||
			pClientImp->m_bWaitSend)
		{
			FD_SET(iter->first, &fdWrite);
		}
		++iter;
	}
	ms_mapLock.unlock();

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000 * nTimeOut;
	int nRet = ::select(0, &fdRead, &fdWrite, NULL, nTimeOut == SELECT_INFINITE ? NULL : &tv);
	if (nRet > 0)
	{
		for (int i = 0; i < selSocks.logicalLength(); ++i)
		{
			SOCKET s = selSocks.at(i);
			ms_mapLock.lock();
			SocketToClientMap::iterator iter = ms_SocketsMap.find(s);
			if (iter == ms_SocketsMap.end())
			{
				ms_mapLock.unlock();
				continue;
			}
			TcpClientSocketImp* pClientImp = iter->second;
			ms_mapLock.unlock();

			if (FD_ISSET(s, &fdRead))
				pClientImp->doRecv();
			if (FD_ISSET(s, &fdWrite))
				pClientImp->doSend();
		}
	}
}

int TcpClientSocketImp::create(bool bBlocking)
{
	if (m_hSocket != INVALID_SOCKET)
		return SEC_SUCCESS;

	m_bBlocking = bBlocking;

	if (!initBuffer(m_nMaxPkgSize))
		return SEC_INTERNAL;

	m_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (!EzVerify(m_hSocket != INVALID_SOCKET))
	{
		int wsaCode = ::WSAGetLastError();
		EzLogError(_T("TcpClientSocketImp::create create socket failed! error code = %d\n"), wsaCode);
		clearBuffer();
		return translateErrCode(wsaCode);
	}

	if (!bBlocking)
	{
#ifdef EZ_WINDOWS
		u_long mode = 1;
		int nRet = ::ioctlsocket(m_hSocket, FIONBIO, &mode);
		if (!EzVerify(nRet == 0))
		{
			int wsaCode = ::WSAGetLastError();
			int errCode = translateErrCode(wsaCode);
			close(errCode);
			EzLogError(_T("convert socket to non-blocking mode failed! error code = %d\n"), wsaCode);
			return errCode;
		}
#else
		// todo
#endif
	}

	m_sockStatus = TcpClientSocket::kCreated;
	ms_mapLock.lock();
	ms_SocketsMap.insert(std::make_pair(m_hSocket, this));
	ms_mapLock.unlock();

	return SEC_SUCCESS;
}

int TcpClientSocketImp::connect(const char* pszAddress, unsigned short uPort)
{
	if (INVALID_SOCKET == m_hSocket)
		return SEC_INVALIDSOCK;

	if (NULL == pszAddress || 0 == *pszAddress)
		return SEC_INVALIDPARAM;

	sockaddr_in serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(pszAddress);
	serverAddr.sin_port = htons(uPort);

	int nRet = ::connect(m_hSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (nRet != 0)
	{
		int wsaErr = ::WSAGetLastError();
		if (WSAEWOULDBLOCK == wsaErr)
		{
			m_sockStatus = TcpClientSocket::kConnecting;
			return SEC_WOULDBLOCK;
		}
		else
		{
			return translateErrCode(wsaErr);
		}
	}

	m_sockStatus = TcpClientSocket::kConnected;
	notifyConnectedEvent();

	return SEC_SUCCESS;
}

int TcpClientSocketImp::sendData(void* pData, size_t nDataLen)
{
	if (INVALID_SOCKET == m_hSocket)
		return SEC_INVALIDSOCK;

	if (NULL == pData || 0 == nDataLen)
		return SEC_INVALIDPARAM;

	if (nDataLen > m_nMaxPkgSize)
		return SEC_INVALIDPKG;

	if (m_bBlocking)
	{
		return sendBlocking(pData, nDataLen);
	}
	else
	{
		if (!copyToSendBuf(pData, nDataLen))
			return SEC_BUFNOTENOUGH;

		return doSend();
	}
}

void TcpClientSocketImp::close(int nErrCode /*= SEC_SUCCESS*/)
{
	m_statusLock.lock();
	if (TcpClientSocket::kClosing == m_sockStatus)
	{
		m_statusLock.unlock();
		return;
	}

	m_sockStatus = TcpClientSocket::kClosing;
	m_statusLock.unlock();

	if (m_hSocket != INVALID_SOCKET)
	{
		ms_mapLock.lock();
		SocketToClientMap::iterator iter = ms_SocketsMap.find(m_hSocket);
		if (iter != ms_SocketsMap.end())
			ms_SocketsMap.erase(iter);
		ms_mapLock.unlock();

		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		notifyClosedEvent(nErrCode);
	}

	clearBuffer();

	m_sockStatus = TcpClientSocket::kInvalid;
}

bool TcpClientSocketImp::addEventHandler(ITcpClientSocketEventHandler* pEventHandler)
{
	if (NULL == pEventHandler)
		return false;

	int idx = m_EventHandlers.find(pEventHandler);
	if (idx < 0)
		m_EventHandlers.append(pEventHandler);
	return true;
}

bool TcpClientSocketImp::removeEventHandler(ITcpClientSocketEventHandler* pEventHandler)
{
	int idx = m_EventHandlers.find(pEventHandler);
	if (idx < 0)
		return false;

	m_EventHandlers.removeAt(idx);
	return true;
}

bool TcpClientSocketImp::initBuffer(size_t nPkgSize)
{
	// there is need not any buffer if socket is in blocking mode.
	if (!m_bBlocking)
	{
		m_nSendBufSize = 2 * (nPkgSize + sizeof(TcpPackageHeader));
		m_pSendBuffer = new char[m_nSendBufSize];
		if (!EzVerify(m_pSendBuffer != NULL))
			return false;
	}

	m_nRecvBufSize = nPkgSize + sizeof(TcpPackageHeader);
	m_pRecvBuffer = new char[m_nRecvBufSize];
	if (!EzVerify(m_pRecvBuffer != NULL))
	{
		EZ_SAFE_DELETE_ARRAY(m_pSendBuffer);
		return false;
	}

	return true;
}

void TcpClientSocketImp::clearBuffer()
{
	EZ_SAFE_DELETE_ARRAY(m_pSendBuffer);
	EZ_SAFE_DELETE_ARRAY(m_pRecvBuffer);
}

bool TcpClientSocketImp::copyToSendBuf(void* pData, size_t nDataLen)
{
	EzAutoLocker _locker(&m_sendLock);

	TcpPackageHeader header;
	header.uPackageSize = nDataLen;

	if (m_nSendBufUsed + sizeof(header) + nDataLen > m_nSendBufSize)
		return false;

	::memcpy(m_pSendBuffer + m_nSendBufUsed, &header, sizeof(header));
	m_nSendBufUsed += sizeof(header);
	::memcpy(m_pSendBuffer + m_nSendBufUsed, pData, nDataLen);
	m_nSendBufUsed += nDataLen;

	return true;
}

int TcpClientSocketImp::sendBlocking(void* pData, size_t nDataLen)
{
	TcpPackageHeader header;
	header.uPackageSize = nDataLen;
	int nRet = ::send(m_hSocket, (const char*)&header, sizeof(header), 0);
	if (SOCKET_ERROR == nRet)
	{
		int nWsaErr = ::WSAGetLastError();
		int errCode = translateErrCode(nWsaErr);
		close(errCode);
		return errCode;
	}

	nRet = ::send(m_hSocket, (const char*)pData, nDataLen, 0);
	if (SOCKET_ERROR == nRet)
	{
		int nWsaErr = ::WSAGetLastError();
		int errCode = translateErrCode(nWsaErr);
		close(errCode);
		return errCode;
	}

	return SEC_SUCCESS;
}

int TcpClientSocketImp::doSend()
{
	EzAutoLocker _locker(&m_sendLock);

	if (TcpClientSocket::kConnecting == m_sockStatus)
	{
		m_sockStatus = TcpClientSocket::kConnected;
		notifyConnectedEvent();
		return SEC_SUCCESS;
	}

	if (m_nSendBufUsed <= 0)
		return SEC_SUCCESS;

	int nRet = ::send(m_hSocket, m_pSendBuffer, m_nSendBufUsed, 0);
	if (SOCKET_ERROR == nRet)
	{
		int wsaErr = ::WSAGetLastError();
		if (WSAEWOULDBLOCK == wsaErr)
		{
			m_bWaitSend = true;
			return SEC_WOULDBLOCK;
		}
		else
		{
			int errCode = translateErrCode(wsaErr);
			close(errCode);
			return errCode;
		}
	}

	if ((size_t)nRet < m_nSendBufUsed)
	{
		::memmove(m_pSendBuffer, m_pSendBuffer + nRet, m_nSendBufUsed - nRet);
		m_nSendBufUsed -= nRet;
	}
	else
	{
		m_bWaitSend = false;
		m_nSendBufUsed = 0;
	}

	return SEC_SUCCESS;
}

int TcpClientSocketImp::doRecv()
{
	do
	{
		int nRet = ::recv(m_hSocket, m_pRecvBuffer + m_nRecvBufUsed, m_nRecvBufSize - m_nRecvBufUsed, 0);
		if (SOCKET_ERROR == nRet)
		{
			int nWsaErr = ::WSAGetLastError();
			if (WSAEWOULDBLOCK == nWsaErr)
			{
				return SEC_WOULDBLOCK;
			}
			else
			{
				int errCode = translateErrCode(nWsaErr);
				close(errCode);
				return errCode;
			}
		}
		else if (0 == nRet)
		{
			close(SEC_CLOSEDBYPEER);
			return SEC_CLOSEDBYPEER;
		}

		m_nRecvBufUsed += nRet;
		while (m_nRecvBufUsed >= sizeof(TcpPackageHeader))
		{
			TcpPackageHeader* pHeader = (TcpPackageHeader*)m_pRecvBuffer;
			size_t nTotalPkgSize = sizeof(TcpPackageHeader) + pHeader->uPackageSize;
			if (m_nRecvBufUsed >= nTotalPkgSize)
			{
				notifyRecvedEvent(pHeader + 1, pHeader->uPackageSize);
				if (m_nRecvBufUsed == nTotalPkgSize)
				{
					m_nRecvBufUsed = 0;
				}
				else
				{
					::memmove(m_pRecvBuffer, m_pRecvBuffer + nTotalPkgSize, m_nRecvBufUsed - nTotalPkgSize);
					m_nRecvBufUsed -= nTotalPkgSize;
				}
			}
			else
			{
				break;
			}
		}
	} while (!m_bBlocking);		// recv only once in blocking mode.

	return true;
}

void TcpClientSocketImp::notifyConnectedEvent()
{
	for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
	{
		ITcpClientSocketEventHandler* pEventHandler = m_EventHandlers[i];
		if (pEventHandler)
			pEventHandler->onSocketConnected(m_pApiObj);
	}
}

void TcpClientSocketImp::notifyRecvedEvent(void* pPackage, size_t nSize)
{
	for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
	{
		ITcpClientSocketEventHandler* pEventHandler = m_EventHandlers[i];
		if (pEventHandler)
			pEventHandler->onSocketRecved(m_pApiObj, pPackage, nSize);
	}
}

void TcpClientSocketImp::notifyClosedEvent(int nErrCode)
{
	for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
	{
		ITcpClientSocketEventHandler* pEventHandler = m_EventHandlers[i];
		if (pEventHandler)
			pEventHandler->onSocketClosed(m_pApiObj, nErrCode);
	}
}

int TcpClientSocketImp::translateErrCode(int nSysCode)
{
	switch (nSysCode)
	{
	case WSAEWOULDBLOCK:
		return SEC_WOULDBLOCK;
	case WSAENETDOWN:
		return SEC_NETDOWN;
	case WSAEINTR:
		return SEC_INTR;
	case WSAEADDRINUSE:
		return SEC_ADDRINUSE;
	case WSAEINPROGRESS:
		return SEC_INPROGRESS;
	case WSAEALREADY:
		return SEC_ALREADY;
	case WSAEADDRNOTAVAIL:
		return SEC_ADDRNOTAVAIL;
	case WSAEAFNOSUPPORT:
		return SEC_AFNOSUPPORT;
	case WSAECONNREFUSED:
		return SEC_CONNREFUSED;
	case WSAEFAULT:
		return SEC_FAULT;
	case WSAEINVAL:
		return SEC_INVAL;
	case WSAEISCONN:
		return SEC_ISCONN;
	case WSAENETUNREACH:
		return SEC_NETUNREACH;
	case WSAENOBUFS:
		return SEC_NOBUFS;
	case WSAENOTSOCK:
		return SEC_NOTSOCK;
	case WSAETIMEDOUT:
		return SEC_TIMEDOUT;
	case WSAEACCES:
		return SEC_ACCES;
	case WSAECONNABORTED:
		return SEC_CONNABORTED;
	case WSAECONNRESET:
		return SEC_CONNRESET;
	}

	return SEC_UNKNOWN;
}

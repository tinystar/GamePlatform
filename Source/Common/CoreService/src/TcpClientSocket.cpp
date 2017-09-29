#include "TcpClientSocket.h"
#include "TcpClientSocketImp.h"
#include "ServiceExcp.h"


TcpClientSocket::TcpClientSocket(size_t nMaxPkgSize /*= 1024*/)
{
	m_pClientSockImp = new TcpClientSocketImp(this, nMaxPkgSize);
	if (!EzVerify(m_pClientSockImp != NULL))
		throw ServiceExcp(_T("new TcpClientSocketImp failed!"), eMemoryNotEnough);
}

TcpClientSocket::~TcpClientSocket()
{
	EZ_SAFE_DELETE(m_pClientSockImp);
}

void TcpClientSocket::select(unsigned int nTimeOut)
{
	TcpClientSocketImp::select(nTimeOut);
}

bool TcpClientSocket::create(bool bBlocking /*= true*/)
{
	return m_pClientSockImp->create(bBlocking);
}

SOCKET TcpClientSocket::getSocket()
{
	return m_pClientSockImp->getSocket();
}

bool TcpClientSocket::connect(const char* pszAddress, unsigned short uPort)
{
	return m_pClientSockImp->connect(pszAddress, uPort);
}

bool TcpClientSocket::sendData(void* pData, size_t nDataLen)
{
	return m_pClientSockImp->sendData(pData, nDataLen);
}

void TcpClientSocket::close()
{
	m_pClientSockImp->close();
}

bool TcpClientSocket::addEventHandler(ITcpClientSocketEventHandler* pEventHandler)
{
	return m_pClientSockImp->addEventHandler(pEventHandler);
}

bool TcpClientSocket::removeEventHandler(ITcpClientSocketEventHandler* pEventHandler)
{
	return m_pClientSockImp->removeEventHandler(pEventHandler);
}

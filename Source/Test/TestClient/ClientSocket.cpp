#include "stdafx.h"
#include "ClientSocket.h"
#include "TcpPackage.h"


ClientSocket::ClientSocket()
	: m_Socket(INVALID_SOCKET)
	, m_bConnected(false)
{
}

ClientSocket::~ClientSocket()
{
// 	close();
}

bool ClientSocket::connectToServer(const char* pAddr, unsigned short sPort)
{
	if (m_bConnected)
		return true;

	m_Socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == m_Socket)
	{
		::MessageBox(NULL, _T("create socket error!"), _T("error"), MB_OK);
		return false;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(pAddr);
	serverAddr.sin_port = htons(sPort);

	int ierr = ::connect(m_Socket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ierr != 0)
	{
		::MessageBox(NULL, _T("connect to server failed!"), _T("error"), MB_OK);
		return false;
	}

	m_bConnected = true;
	return true;
}

bool ClientSocket::sendPackage(const char* pData, size_t nDataSize)
{
	if (!m_bConnected)
		return false;

// 	char szBuffer[1024] = { 0 };
// 	TcpPackageHeader* pHeader = (TcpPackageHeader*)szBuffer;
// 	pHeader->uPackageSize = nDataSize;
// 	::memcpy(pHeader + 1, pData, nDataSize);

	int state = ::send(m_Socket, (const char*)&nDataSize, sizeof(TcpPackageHeader), 0);
	if (state <= 0)
		return false;

	return ::send(m_Socket, pData, nDataSize, 0) != SOCKET_ERROR;
}

bool ClientSocket::recvPackage(char* pBuffer, size_t nBufSize)
{
	if (!m_bConnected)
		return false;

	char szPkgHeader[10] = { 0 };

	int iRet = ::recv(m_Socket, szPkgHeader, sizeof(TcpPackageHeader), 0);
	if (iRet <= 0)
		return false;

	TcpPackageHeader* pHeader = (TcpPackageHeader*)szPkgHeader;
	//TRACE(_T("client: recv header %d\n"), pHeader->uPackageSize);

	if (pHeader->uPackageSize > nBufSize)
	{
		::MessageBox(NULL, _T("buffer is too small to recv the package!"), _T("error"), MB_OK);
		return false;
	}

	int iTotalRecv = 0;
	iRet = 0;
	do 
	{
		iRet = ::recv(m_Socket, pBuffer + iTotalRecv, pHeader->uPackageSize - iTotalRecv, 0);
		if (iRet <= 0)
			return false;

		iTotalRecv += iRet;
		//TRACE(_T("client: recv body %d\n"), iRet);
	} while (iTotalRecv != pHeader->uPackageSize);

	return true;
}

bool ClientSocket::close()
{
	if (m_Socket != INVALID_SOCKET)
	{
		::closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	return true;
}

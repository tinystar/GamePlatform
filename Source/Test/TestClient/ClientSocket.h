#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include <WinSock2.h>

class ClientSocket
{
public:
	ClientSocket();
	~ClientSocket();

public:
	bool connectToServer(const char* pAddr, unsigned short sPort);
	bool sendPackage(const char* pData, size_t nDataSize);
	bool recvPackage(char* pBuffer, size_t nBufSize);

	bool close();

protected:
	SOCKET	m_Socket;
	bool	m_bConnected;
};

#endif // __CLIENT_SOCKET_H__
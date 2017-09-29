/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/17
/************************************************************************/
#ifndef __TCP_CLIENTSOCKET_H__
#define __TCP_CLIENTSOCKET_H__

#include "CoreService.h"


#define SELECT_INFINITE			((unsigned int)-1)	// infinite timeout for the parameter of TcpClientSocket::select function

class TcpClientSocket;
class TcpClientSocketImp;

// TCP client socket event notify interface
struct ITcpClientSocketEventHandler
{
	virtual void onSocketConnected(TcpClientSocket* pClientSock) = 0;
	virtual void onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize) = 0;
	virtual void onSocketClosed(TcpClientSocket* pClientSock) = 0;
};

class SVC_DLL_SPEC TcpClientSocket
{
EZ_FORBID_CLASS_COPY(TcpClientSocket)

public:
	explicit TcpClientSocket(size_t nMaxPkgSize = 1024);
	~TcpClientSocket();

public:
	static void select(unsigned int nTimeOut);

public:
	SOCKET getSocket();

	bool create(bool bBlocking = true);
	bool connect(const char* pszAddress, unsigned short uPort);
	bool sendData(void* pData, size_t nDataLen);
	void close();

public:
	bool addEventHandler(ITcpClientSocketEventHandler* pEventHandler);
	bool removeEventHandler(ITcpClientSocketEventHandler* pEventHandler);

private:
	TcpClientSocketImp*		m_pClientSockImp;
};

#endif // __TCP_CLIENTSOCKET_H__
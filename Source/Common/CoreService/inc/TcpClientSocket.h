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

enum SocketErrCode
{
	SEC_SUCCESS = 0,
	SEC_INTERNAL,
	SEC_INVALIDSOCK,
	SEC_INVALIDPARAM,
	SEC_INVALIDPKG,
	SEC_BUFNOTENOUGH,
	SEC_CLOSEDBYPEER,
	SEC_WOULDBLOCK,
	SEC_NETDOWN,
	SEC_INTR,
	SEC_ADDRINUSE,
	SEC_INPROGRESS,
	SEC_ALREADY,
	SEC_ADDRNOTAVAIL,
	SEC_AFNOSUPPORT,
	SEC_CONNREFUSED,
	SEC_FAULT,
	SEC_INVAL,
	SEC_ISCONN,
	SEC_NETUNREACH,
	SEC_NOBUFS,
	SEC_NOTSOCK,
	SEC_TIMEDOUT,
	SEC_ACCES,
	SEC_CONNABORTED,
	SEC_CONNRESET,
	SEC_UNKNOWN
};

class TcpClientSocket;
class TcpClientSocketImp;

// TCP client socket event notify interface
struct ITcpClientSocketEventHandler
{
	virtual void onSocketConnected(TcpClientSocket* pClientSock) = 0;
	virtual void onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize) = 0;
	virtual void onSocketClosed(TcpClientSocket* pClientSock, int nErrCode) = 0;
};

class SVC_DLL_SPEC TcpClientSocket
{
EZ_FORBID_CLASS_COPY(TcpClientSocket)

public:
	enum Status
	{
		kInvalid = 0,
		kCreated,
		kConnecting,
		kConnected,
		kClosing
	};

public:
	explicit TcpClientSocket(size_t nMaxPkgSize = 1024);
	~TcpClientSocket();

public:
	static void select(unsigned int nTimeOut);

public:
	SOCKET getSocket() const;
	Status getStatus() const;

	int create(bool bBlocking = true);
	int connect(const char* pszAddress, unsigned short uPort);
	int sendData(void* pData, size_t nDataLen);
	void close();

public:
	bool addEventHandler(ITcpClientSocketEventHandler* pEventHandler);
	bool removeEventHandler(ITcpClientSocketEventHandler* pEventHandler);

private:
	TcpClientSocketImp*		m_pClientSockImp;
};

#endif // __TCP_CLIENTSOCKET_H__
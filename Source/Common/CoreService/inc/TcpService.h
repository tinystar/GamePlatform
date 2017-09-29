/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/11/28
/************************************************************************/
#ifndef __TCP_SERVICE_H__
#define __TCP_SERVICE_H__

#include "CoreService.h"
#include "ClientId.h"
#include "ClientIdIterator.h"


enum NetModel
{
	kIocp = 0,							// can only be used in windows.
};

struct TcpConfig
{
	char			szAddr[20];			// if empty, INADDR_ANY is used when bind to the address.
	unsigned short	sPort;
	unsigned int	nSockThreadCnt;		// if 0, then the count of socket threads is 2*(num of cpu)
	unsigned int	nMaxAcceptCnt;		// max posted accept i/o operation count.
	size_t			nMaxPackageSize;	// max package size, it's also determines the size of send/recv buffer.

	SVC_DLL_SPEC static const TcpConfig kDefault;

	TcpConfig()
		: sPort(5000)
		, nSockThreadCnt(0)
		, nMaxAcceptCnt(10)
		, nMaxPackageSize(1024)
	{
		::memset(szAddr, 0, sizeof(szAddr));
	}
};

struct TcpServiceClientFilter
{
	// the function is called when a client socket is accepted and it's return value
	// is used to determinate whether the client can be connected.
	// return false to refuse the client socket.
	virtual bool filterNewClient(SOCKET sockClient, SOCKADDR_IN* pClientAddr) { return true; }
};

// TCP event notify interface
struct ITcpServiceEventHandler
{
	virtual void onClientConnected(ClientId id) = 0;
	virtual void onPackageReceived(ClientId id, void* pPackage, size_t nSize) = 0;
	virtual void onClientClosed(ClientId id) = 0;
};

class TcpService
{
EZ_FORBID_CLASS_COPY(TcpService)

public:
	TcpService();
	virtual ~TcpService();

public:
	virtual SVCErrorCode init(const TcpConfig& config = TcpConfig::kDefault) = 0;
	virtual SVCErrorCode unInit() = 0;
	virtual SVCErrorCode start() = 0;
	virtual SVCErrorCode stop() = 0;

	virtual bool sendData(ClientId id, void* pData, size_t nDataLen) = 0;
	virtual bool sendDataToAll(void* pData, size_t nDataLen) = 0;

	virtual bool closeClient(ClientId id, bool bGraceful = true) = 0;

	virtual size_t getClientCount() const = 0;

	// must be deleted by the caller.
	virtual ClientIdIterator* newIterator() const = 0;

	// for debug
	virtual void dump(EzDumpContext& dc) const = 0;

public:
	bool addEventHandler(ITcpServiceEventHandler* pEventHandler);
	bool removeEventHandler(ITcpServiceEventHandler* pEventHandler);

	bool registerClientFilter(TcpServiceClientFilter* pClientFilter);

protected:
	typedef EzArray<ITcpServiceEventHandler*>	EventHandlerArray;

	EventHandlerArray			m_EventHandlers;
	TcpServiceClientFilter*		m_pClientFilter;
};

SVC_DLL_SPEC TcpService* createTcpService(NetModel model = kIocp);
SVC_DLL_SPEC void releaseTcpService(TcpService* pService);

#endif // __TCP_SERVICE_H__
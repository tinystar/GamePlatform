/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/3/20
/************************************************************************/
#ifndef __SERVER_TEMPLATE_H__
#define __SERVER_TEMPLATE_H__

#include "ServerKernel.h"
#include "TcpService.h"
#include "TimerService.h"


enum eServiceCreateFlags
{
	kCreateTcpService	= 0x00000001,
	kCreateTimerService = 0x00000002,

	kCreateAllServices	= kCreateTcpService | kCreateTimerService
};

struct ServerInitConfig
{
	EzUInt32		uCreateFlags;
	TcpConfig		tcpConfig;

	ServerInitConfig()
		: uCreateFlags(kCreateAllServices)
	{
	}
};

// server base class
// you may inherit this class to implement your own server logic.
// using template method pattern to let derived class to customize itself.
class SK_DLL_SPEC ServerTemplate
{
	friend class ServerTemplateImp;

EZ_FORBID_CLASS_COPY(ServerTemplate)

public:
	ServerTemplate();
	virtual ~ServerTemplate();

public:
	SVCErrorCode init(const ServerInitConfig& serverConfig);
	SVCErrorCode unInit();
	SVCErrorCode start();
	SVCErrorCode stop();

public:
	// add a user defined item to the producer/consumer queue.
	// The item will be retrieved from the queue later and be
	// dispatched through the 'onUserItemMsg' virtual function.
	// You can override the virtual function to process the item.
	bool queueUserItem(int itemId, void* pData, size_t nSize);

protected:
	bool setTimer(EzUInt uTimerId, EzUInt uElapse);
	bool killTimer(EzUInt uTimerId);

	bool sendData(ClientId id, void* pData, size_t nDataLen);
	bool sendDataToAll(void* pData, size_t nDataLen);

	// factory methods
protected:
	virtual TcpService* createTcpService() const;
	virtual TimerService* createTimerService() const;

	// template methods
protected:
	virtual bool onInit(const ServerInitConfig&) { return true; }
	virtual bool onUninit() { return true; }
	virtual bool onStart() { return true; }
	virtual bool onStop() { return true; }

	// event handler methods, must be override
protected:
	virtual void onTcpClientConnectMsg(ClientId id) = 0;
	virtual void onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize) = 0;
	virtual void onTcpClientCloseMsg(ClientId id) = 0;

	virtual void onTimerMsg(EzUInt uTimerId) = 0;

	virtual void onUserItemMsg(int itemId, void* pData, size_t nSize) = 0;

protected:
	ServerTemplateImp*		m_pServerImp;
	TcpService*				m_pTcpService;
	TimerService*			m_pTimerService;

	EzUInt32				m_state;
};

#endif // __SERVER_TEMPLATE_H__
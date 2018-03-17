/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/7/31
/************************************************************************/
#ifndef __BASE_GAME_SERVER_H__
#define __BASE_GAME_SERVER_H__

#include "GameBase.h"
#include "ServerTemplate.h"
#include "NetMsgMapDefs.h"
#include <map>


class BaseGameServer : public ServerTemplate
{
public:
	BaseGameServer();
	virtual ~BaseGameServer();

protected:
	virtual bool onInit(const ServerInitConfig& serverConfig);
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();

protected:
	virtual void onTcpClientConnectMsg(ClientId id);
	virtual void onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize);
	virtual void onTcpClientCloseMsg(ClientId id);

	virtual void onTimerMsg(EzUInt uTimerId);

	virtual void onUserItemMsg(int itemId, void* pData, size_t nSize);

protected:
	// msg will be encrypted if there has an msg encryptor.
	bool sendMsg(ClientId id, void* pData, size_t nDataLen);
	bool sendMsg(ClientId id, CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved = 0);
	bool sendMsgToAll(void* pData, size_t nDataLen);
	bool sendMsgToAll(CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved = 0);

	bool registerMsgHandler(CSUINT16 uMainId, CSUINT16 uSubId, NetMsgHandler pHandler);
	bool removeMsgHandler(CSUINT16 uMainId, CSUINT16 uSubId);

	// input parameter must be dynamically allocated and needn't be deleted,
	// it will be automatically deleted in the destructor of this class.
	void setMsgEncryptor(IEzEncryptor* pEncryptor);

protected:
	// Msg that are not mapped by the msg map mechanism will be handled by this function.
	virtual void onDefaultMsgHandler(ClientId id, void* pData, size_t nDataLen);

	DECLARE_NETMSG_TABLE()

private:
	void buildNetMsgMap();
	void clearNetMsgMap();
	void registerMsgHandler(const NetMsgTable* pMsgTable);

protected:
	typedef std::map<CSUINT32, NetMsgHandler> NetMsgMap;

	NetMsgMap			m_NetMsgMap;
	IEzEncryptor*		m_pMsgEncryptor;
};

#endif // __BASE_GAME_SERVER_H__

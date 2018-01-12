/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/9
/************************************************************************/
#ifndef __MAIN_SERVER_H__
#define __MAIN_SERVER_H__

#include "BaseGameServer.h"
#include "TcpClientSocket.h"
#include "IMainServerUIObserver.h"
#include <list>
#include <map>

class MainServer : public BaseGameServer, public ITcpClientSocketEventHandler
{
public:
	MainServer();
	virtual ~MainServer();

public:
	void setGateSvrAddr(const char* pszAddr, unsigned short sPort);
	void setDBSvrAddr(const char* pszAddr, unsigned short sPort);
	void setMaxUser(unsigned int uMax) { m_uMaxUser = uMax; }

	const char* getGateSvrIP() const { return m_szGateAddr; }
	const char* getDBSvrIP() const { return m_szDBAddr; }
	unsigned short getGateSvrPort() const { return m_sGatePort; }
	unsigned short getDBSvrPort() const { return m_sDBPort; }
	unsigned int getMaxUser() const { return m_uMaxUser; }

	void registerUIObserver(IMainServerUIObserver* pObserver) { m_pUIObserver = pObserver; }

protected:
	virtual bool onInit(const ServerInitConfig& serverConfig);
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();

protected:
	virtual void onSocketConnected(TcpClientSocket* pClientSock);
	virtual void onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize);
	virtual void onSocketClosed(TcpClientSocket* pClientSock, int nErrCode);

	virtual void onTcpClientCloseMsg(ClientId id);

	virtual void onTimerMsg(EzUInt uTimerId);

	virtual void onUserItemMsg(int itemId, void* pData, size_t nSize);

protected:
	bool connectToGate();
	bool connectToDB();

	void onGateServerMsg(void* pData, size_t nSize);
	void onDBServerMsg(void* pData, size_t nSize);

	bool sendMsgToServer(TcpClientSocket* pClientSock, void* pData, size_t nDataLen);
	bool sendMsgToServer(TcpClientSocket* pClientSock, CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved = 0);

	bool addLoginUser(ClientId cId, const UserInfo& userInfo);
	bool removeLoginUser(ClientId cId);

	ClientId findClientByUserId(EzUInt32 userId) const;

	bool removeClientFromDBReqQueue(ClientId id);

protected:
	static unsigned __stdcall clientSelectThread(void* pParam);

	static EzULong genQuickLoginStamp();

	// client messages
protected:
	void onAccountLogin(ClientId id, void* pData, size_t nDataLen);
	void onQuickLogin(ClientId id, void* pData, size_t nDataLen);

	// DBServer messages
protected:
	void onDBCreateGuestFail(void* pData, size_t nSize);
	void onDBCreateGuestSucc(void* pData, size_t nSize);

protected:
	typedef std::list<ClientStamp>			ClientStampQueue;
	typedef std::map<EzUInt32, ClientId>	UserId2ClientIdMap;

	static NetMsgMapEntry s_msgMapArray[];

	char					m_szGateAddr[20];
	char					m_szDBAddr[20];
	unsigned short			m_sGatePort;
	unsigned short			m_sDBPort;
	unsigned short			m_sPort;
	unsigned int			m_uMaxUser;
	char					m_szSvrName[256];	// �������ݲ�ʹ��

	TcpClientSocket			m_clientToGate;
	TcpClientSocket			m_clientToDB;
	HANDLE					m_hSelectThread;
	bool					m_bStopServer;

	ClientStampQueue		m_reqToDBClientQueue;

	UserId2ClientIdMap		m_userIdToClientMap;

	IMainServerUIObserver*	m_pUIObserver;
};

#endif // __MAIN_SERVER_H__

/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/11/26
/************************************************************************/
#ifndef __GAME_SERVER_MANAGER_H__
#define __GAME_SERVER_MANAGER_H__

#include "GameSvrMgr.h"
#include "GameNode.h"
#include "TcpClientSocket.h"

struct GSM_DLL_SPEC MgrInitConfig
{
	bool			bDebugMode;
	char			szDBAddress[20];
	unsigned short	sDBPort;
	size_t			nRoomPkgSize;

	MgrInitConfig()
		: bDebugMode(false)
		, sDBPort(0)
		, nRoomPkgSize(1024)
	{
		::memset(szDBAddress, 0, sizeof(szDBAddress));
	}
};

class GSM_DLL_SPEC GameServerManager : public ITcpClientSocketEventHandler
{
public:
	// singleton
	static GameServerManager* getInstance();
	static void destroyInstance();

public:
	bool initialize(const MgrInitConfig& config);
	bool unInitialize();

	void updateGameList();
	bool loadGameModule(GameKind* pGameKind);
	bool unloadGameModule(GameKind* pGameKind);
	bool startGameRoom(GameRoom* pGameRoom);
	bool stopGameRoom(GameRoom* pGameRoom);

	const GameListTree& getGameList() const { return m_gameList; }

	void doUpdate();

protected:
	virtual void onSocketConnected(TcpClientSocket* pClientSock);
	virtual void onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize);
	virtual void onSocketClosed(TcpClientSocket* pClientSock, int nErrCode);

protected:
	bool connectToDB();

	bool sendMsgToServer(TcpClientSocket* pClientSock, void* pData, size_t nDataLen);
	bool sendMsgToServer(TcpClientSocket* pClientSock, CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved = 0);

	void onDBQueryGameInfoFail(void* pData, size_t nSize);
	void onDBQueryGameKinds(void* pData, size_t nSize);
	void onDBQueryGamePlaces(void* pData, size_t nSize);
	void onDBQueryGameRooms(void* pData, size_t nSize);

private:
	GameServerManager();
	~GameServerManager();

private:
	static GameServerManager*	m_pSharedManager;

	MgrInitConfig				 m_config;
	TcpClientSocket				 m_clientToDB;
	GameListTree				 m_gameList;
};

#define GameServerMgr	GameServerManager::getInstance

#endif // __GAME_SERVER_MANAGER_H__

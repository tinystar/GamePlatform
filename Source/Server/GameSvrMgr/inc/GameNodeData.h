/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/2/19
/************************************************************************/
#ifndef __GAME_NODE_DATA_H__
#define __GAME_NODE_DATA_H__

#include "xEzUtil.h"

typedef EzSys::HEZMODULE	HGMMODULE;

class GameRoomServer;
struct RoomContext;
struct ServerInitConfig;


class GameModuleMgr
{
EZ_FORBID_CLASS_COPY(GameModuleMgr)

public:
	GameModuleMgr();
	~GameModuleMgr();

	HGMMODULE getModuleHandle() const { return m_hGameModule; }
	bool isLoaded() const { return m_hGameModule != NULL; }

	bool loadGameModule(const char* pszModName);
	bool unloadGameModule();

	void* getSymbAddress(const char* pszSymbName) const;

private:
	HGMMODULE	m_hGameModule;
};

class GameRoomMgr
{
EZ_FORBID_CLASS_COPY(GameRoomMgr)

public:
	explicit GameRoomMgr(const RoomContext& ctx);
	~GameRoomMgr();

	GameRoomServer* getRoomServer() const { return m_pRoomServer; }
	bool isRunning() const { return m_bRunning; }

	bool startRoomServer(const ServerInitConfig& config);
	bool stopRoomServer();

protected:
	bool createRoomServer(const RoomContext& ctx);
	bool destroyRoomServer();

private:
	GameRoomServer*	m_pRoomServer;
	bool			m_bRunning;
};

#endif // __GAME_NODE_DATA_H__

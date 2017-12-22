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

#include "GameNode.h"

class GameServerManager
{
public:
	// singleton
	static GameServerManager* getInstance();
	static void destroyInstance();

public:
	bool updateGameList();
	bool loadGameModule(GameKind* pGameKind);
	bool unloadGameModule(GameKind* pGameKind);
	bool startGameRoom(GameRoom* pGameRoom);
	bool stopGameRoom(GameRoom* pGameRoom);

private:
	GameServerManager();
	~GameServerManager();

private:
	static GameServerManager*	m_pSharedManager;

	//GameRoot					m_rootNode;
};

#endif // __GAME_SERVER_MANAGER_H__

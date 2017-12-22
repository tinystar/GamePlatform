/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/11/27
/************************************************************************/
#ifndef __GAME_ROOM_SERVER_H__
#define __GAME_ROOM_SERVER_H__

#include "BaseGameServer.h"

class GameDesk;

class GameRoomServer : public BaseGameServer
{
public:
	GameRoomServer();
	virtual ~GameRoomServer();

protected:
	GameDesk*		m_pGameDesks;
};

#endif // __GAME_ROOM_SERVER_H__

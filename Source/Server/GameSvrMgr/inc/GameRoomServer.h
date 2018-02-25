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
#include "GameNodeInfo.h"

class GameDesk;
class GameDeskFactory;

struct RoomContext
{
	PlaceType		 placeType;
	double			 dEnterLimit;
	double			 dBasePoint;
	GameDeskFactory* pDeskFactory;

	RoomContext()
		: placeType(kPlaceNormal)
		, dEnterLimit(0.0)
		, dBasePoint(0.0)
		, pDeskFactory(NULL)
	{}
};

class GameRoomServer : public BaseGameServer
{
public:
	explicit GameRoomServer(const RoomContext& ctx);
	virtual ~GameRoomServer();

protected:
	virtual bool onInit(const ServerInitConfig& serverConfig);
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();

protected:
	GameDesk*			m_pGameDesks;
	const RoomContext	m_RoomCtx;
};

#endif // __GAME_ROOM_SERVER_H__

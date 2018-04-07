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
#include "RoomPlayerManager.h"

class GameDesk;
class GameDeskFactory;

struct RoomContext
{
	PlaceType		 placeType;
	double			 dEnterLimit;
	double			 dBasePoint;
	EzUInt32		 uMaxUserCount;
	GameDeskFactory* pDeskFactory;

	RoomContext()
		: placeType(kPlaceNormal)
		, dEnterLimit(0.0)
		, dBasePoint(0.0)
		, uMaxUserCount(0)
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
	bool initGameDesks();
	void cleanGameDesks();

protected:
	const RoomContext	m_RoomCtx;
	GameDesk**			m_pGameDesks;
	int					m_nDeskCount;
	RoomPlayerManager	m_playerManager;
};

#endif // __GAME_ROOM_SERVER_H__

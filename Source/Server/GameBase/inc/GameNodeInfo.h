/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/12/9
/************************************************************************/
#ifndef __GAME_NODE_INFO_H__
#define __GAME_NODE_INFO_H__

#include "xEzTypes.h"
#include <memory.h>
#include "BaseMsgDefs.h"
#include "GameBase.h"

enum PlaceType
{
	kPlaceNormal	= 0,
	kPlaceFriend	= 1
};


struct GB_DLL_SPEC GameKindInfo
{
	EzInt32			nKindId;
	char			szGameName[24];
	char			szClientModule[24];
	char			szServerModule[24];
	char			szVersion[16];

	GameKindInfo()
		: nKindId(-1)
	{
		::memset(szGameName, 0, sizeof(szGameName));
		::memset(szClientModule, 0, sizeof(szClientModule));
		::memset(szServerModule, 0, sizeof(szServerModule));
		::memset(szVersion, 0, sizeof(szVersion));
	}

	void initWithGameKindMsg(const GameKindMsgInfo& kindInfo);
	void setGameKindMsg(GameKindMsgInfo& kindInfo);
};

struct GB_DLL_SPEC GamePlaceInfo
{
	EzInt32			nPlaceId;
	char			szPlaceName[24];
	PlaceType		placeType;
	double			dEnterLimit;
	double			dBasePoint;

	GamePlaceInfo()
		: nPlaceId(-1)
		, placeType(kPlaceNormal)
		, dEnterLimit(0.0)
		, dBasePoint(0.0)
	{
		::memset(szPlaceName, 0, sizeof(szPlaceName));
	}

	void initWithGamePlaceMsg(const GamePlaceMsgInfo& placeInfo);
	void setGamePlaceMsg(GamePlaceMsgInfo& placeInfo, EzInt32 nKindId);
};

struct GB_DLL_SPEC GameRoomInfo
{
	EzInt32			nRoomId;
	char			szRoomName[24];
	char			szServerIp[20];
	unsigned short	sServerPort;

	GameRoomInfo()
		: nRoomId(-1)
		, sServerPort(0)
	{
		::memset(szRoomName, 0, sizeof(szRoomName));
		::memset(szServerIp, 0, sizeof(szServerIp));
	}

	void initWithGameRoomMsg(const GameRoomMsgInfo& roomInfo);
	void setGameRoomMsg(GameRoomMsgInfo& roomInfo, EzInt32 nKindId, EzInt32 nPlaceId);
};

#endif // __GAME_NODE_INFO_H__

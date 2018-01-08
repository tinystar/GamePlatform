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


struct GameKindInfo
{
	EzUInt32		uKindId;
	char			szGameName[24];
	char			szClientModule[24];
	char			szServerModule[24];
	char			szVersion[16];
};

struct GamePlaceInfo
{
	EzUInt32		uPlaceId;
	char			szPlaceName[24];
	double			dEnterLimit;
	double			dBasePoint;
};

struct GameRoomInfo
{
	EzUInt32		uRoomId;
	char			szRoomName[24];
	char			szServerIp[20];
	unsigned short	sServerPort;
};

#endif // __GAME_NODE_INFO_H__

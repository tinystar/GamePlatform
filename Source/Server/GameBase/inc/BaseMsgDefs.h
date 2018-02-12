/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/8/1
/************************************************************************/
#ifndef __BASE_MSG_DEFS_H__
#define __BASE_MSG_DEFS_H__

#include "ServiceTypes.h"
#include <memory.h>
#include "ClientId.h"

#pragma pack(push, 1)

//-------------------------------------------------------------------------------
// Common struct
//-------------------------------------------------------------------------------
struct UserInfo
{
	CSUINT32		userId;
	char			szAccount[17];
	char			szUserName[65];
	CSINT32			headIdx;
	CSINT32			genderType;
	CSUINT32		uMoney;				// 金钱用double表示，但是在网络传输时采用整形（浮点型不同语言、编译器编码方式可能不一样，网络传输最好是整形）
	CSUINT32		uRoomCard;
	char			szPhoneNum[16];
	CSUINT32		uTypeFlag;

	UserInfo()
		: userId(0)
		, headIdx(0)
		, genderType(0)
		, uMoney(0)
		, uRoomCard(0)
		, uTypeFlag(0)
	{
		::memset(szAccount, 0, sizeof(szAccount));
		::memset(szUserName, 0, sizeof(szUserName));
		::memset(szPhoneNum, 0, sizeof(szPhoneNum));
	}
};

// 唯一标志客户端连接，主要用于玩家登录期间DB服务器和其它服务器通信时唯一确定玩家客户端连接
struct ClientStamp
{
	ClientId		clientId;
	CSULONG			ulStamp;

	ClientStamp()
		: ulStamp(0)
	{}

	bool operator== (const ClientStamp& rhs) const
	{
		return clientId == rhs.clientId && ulStamp == rhs.ulStamp;
	}
};

struct GameKindMsgInfo
{
	CSINT32			nKindId;
	char			szGameName[24];
	char			szCliModInfo[24];
	char			szSvrModInfo[24];
	char			szVersion[16];

	GameKindMsgInfo()
		: nKindId(-1)
	{
		::memset(szGameName, 0, sizeof(szGameName));
		::memset(szCliModInfo, 0, sizeof(szCliModInfo));
		::memset(szSvrModInfo, 0, sizeof(szSvrModInfo));
		::memset(szVersion, 0, sizeof(szVersion));
	}
};

struct GamePlaceMsgInfo
{
	CSINT32			nKindId;
	CSINT32			nPlaceId;
	char			szPlaceName[24];
	CSINT32			nPlaceType;
	CSUINT32		uEnterLimit;	// 金钱用double表示，但是在网络传输时采用整形（浮点型不同语言、编译器编码方式可能不一样，网络传输最好是整形）
	CSUINT32		uBasePoint;		// 金钱用double表示，但是在网络传输时采用整形（浮点型不同语言、编译器编码方式可能不一样，网络传输最好是整形）

	GamePlaceMsgInfo()
		: nKindId(-1)
		, nPlaceId(-1)
		, nPlaceType(0)
		, uEnterLimit(0)
		, uBasePoint(0)
	{
		::memset(szPlaceName, 0, sizeof(szPlaceName));
	}
};

struct GameRoomMsgInfo
{
	CSINT32			nKindId;
	CSINT32			nPlaceId;
	CSINT32			nRoomId;
	char			szRoomName[24];
	char			szServerIp[20];
	CSUINT16		sServerPort;

	GameRoomMsgInfo()
		: nKindId(-1)
		, nPlaceId(-1)
		, nRoomId(-1)
		, sServerPort(0)
	{
		::memset(szRoomName, 0, sizeof(szRoomName));
		::memset(szServerIp, 0, sizeof(szServerIp));
	}
};


//-------------------------------------------------------------------------------
// Net message definitions
//-------------------------------------------------------------------------------
struct GameMsgHeader
{
	CSUINT16	uMainId;
	CSUINT16	uSubId;
	CSUINT32	uReserved;

	GameMsgHeader()
		: uMainId(0)
		, uSubId(0)
		, uReserved(0)
	{}
};

struct ClientStampMsg
{
	GameMsgHeader	header;
	ClientStamp		clientStamp;
};

struct UserInfoMsg
{
	GameMsgHeader	header;
	UserInfo		userInfo;
};

struct UserInfoWithClientMsg
{
	GameMsgHeader	header;
	ClientStamp		clientStamp;
	UserInfo		userInfo;
};

struct GameKindListMsg
{
	GameMsgHeader	header;
	CSUINT32		uCount;
	GameKindMsgInfo	kinds[1];	// not zero length, avoid warning in MSVC compiler

	GameKindListMsg()
		: uCount(0)
	{}
};

struct GamePlaceListMsg
{
	GameMsgHeader		header;
	CSUINT32			uCount;
	GamePlaceMsgInfo	places[1];

	GamePlaceListMsg()
		: uCount(0)
	{}
};

struct GameRoomListMsg
{
	GameMsgHeader	header;
	CSUINT32		uCount;
	GameRoomMsgInfo	rooms[1];

	GameRoomListMsg()
		: uCount(0)
	{}
};

#pragma pack(pop)

#endif // __BASE_MSG_DEFS_H__

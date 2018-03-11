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
	CSUINT32		uMoney;				// ��Ǯ��double��ʾ�����������紫��ʱ�������Σ������Ͳ�ͬ���ԡ����������뷽ʽ���ܲ�һ�������紫����������Σ�
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
	CSUINT32		uEnterLimit;	// ��Ǯ��double��ʾ�����������紫��ʱ�������Σ������Ͳ�ͬ���ԡ����������뷽ʽ���ܲ�һ�������紫����������Σ�
	CSUINT32		uBasePoint;		// ��Ǯ��double��ʾ�����������紫��ʱ�������Σ������Ͳ�ͬ���ԡ����������뷽ʽ���ܲ�һ�������紫����������Σ�

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
	CSUINT32		uMaxUser;

	GameRoomMsgInfo()
		: nKindId(-1)
		, nPlaceId(-1)
		, nRoomId(-1)
		, sServerPort(0)
		, uMaxUser(0)
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

struct UserInfoMsg
{
	GameMsgHeader	header;
	UserInfo		userInfo;
};

struct UserInfoWithClientMsg
{
	GameMsgHeader	header;
	ClientId		clientId;
	CSHANDLE		clientHandle;
	UserInfo		userInfo;

	UserInfoWithClientMsg()
		: clientHandle(0)
	{}
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

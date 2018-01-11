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
	CSINT32			genderType;
	CSUINT32		uMoney;				// 金钱用double表示，但是在网络传输时采用整形（浮点型不同语言、编译器编码方式可能不一样，网络传输最好是整形）
	CSUINT32		uRoomCard;
	char			szPhoneNum[16];
	CSUINT32		uTypeFlag;

	UserInfo()
		: userId(0)
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

#pragma pack(pop)

#endif // __BASE_MSG_DEFS_H__

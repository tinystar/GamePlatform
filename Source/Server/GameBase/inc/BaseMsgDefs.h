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

#pragma pack(push, 1)

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


// 唯一标志客户端连接的消息，主要用于玩家登录期间DB服务器返回消息时确定是哪个玩家
struct ClientStampMsg
{
	GameMsgHeader	header;
	ClientId		clientId;
	CSULONG			ulStamp;

	ClientStampMsg()
		: ulStamp(0)
	{
	}
};

struct UserInfoMsg
{
	GameMsgHeader	header;
	CSUINT32		userId;
	char			szAccount[17];
	char			szUserName[65];
	CSINT32			genderType;
	CSUINT32		uMoney;				// 金钱用double表示，但是在网络传输时采用整形（浮点型不同语言、编译器编码方式可能不一样，网络传输最好是整形）
	CSUINT32		uRoomCard;
	char			szPhoneNum[16];
	CSUINT32		uTypeFlag;

	UserInfoMsg()
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

struct UserInfoWithClientMsg
{
	GameMsgHeader	header;
	CSUINT32		userId;
	ClientId		clientId;
	CSULONG			ulStamp;

	char			szAccount[17];
	char			szUserName[65];
	CSINT32			genderType;
	CSUINT32		uMoney;				// 金钱用double表示，但是在网络传输时采用整形（浮点型不同语言、编译器编码方式可能不一样，网络传输最好是整形）
	CSUINT32		uRoomCard;
	char			szPhoneNum[16];
	CSUINT32		uTypeFlag;

	UserInfoWithClientMsg()
		: ulStamp(0)
		, userId(0)
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

#pragma pack(pop)

#endif // __BASE_MSG_DEFS_H__

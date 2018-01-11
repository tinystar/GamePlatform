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
	CSUINT32		uMoney;				// ��Ǯ��double��ʾ�����������紫��ʱ�������Σ������Ͳ�ͬ���ԡ����������뷽ʽ���ܲ�һ�������紫����������Σ�
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

// Ψһ��־�ͻ������ӣ���Ҫ������ҵ�¼�ڼ�DB������������������ͨ��ʱΨһȷ����ҿͻ�������
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

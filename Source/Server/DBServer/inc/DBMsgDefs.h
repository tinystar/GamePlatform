/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/4
/************************************************************************/
#ifndef __DB_MSG_DEFS_H__
#define __DB_MSG_DEFS_H__

#include "BaseMsgDefs.h"

#define MSG_MAINID_DB						5

#define MSG_SUBID_CREATE_GUEST_ACCT			1
#define MSG_SUBID_LOGIN_MAIN_BY_ACCOUNT		2
#define MSG_SUBID_LOGIN_MAIN_BY_USERID		3
#define MSG_SUBID_DB_LOGIN_SUCCESS			4
#define MSG_SUBID_DB_LOGIN_FAILURE			5
#define MSG_SUBID_LOGOUT_MAIN				6

#define MSG_SUBID_QUERY_GAMEKINDS			11
#define MSG_SUBID_QUERY_GAMEPLACES			12
#define MSG_SUBID_QUERY_GAMEROOMS			13
#define MSG_SUBID_GET_GAMEINFO_FAIL			14
#define	MSG_SUBID_GET_GAMEKIND_SUCC			15
#define	MSG_SUBID_GET_GAMEPLACE_SUCC		16
#define MSG_SUBID_GET_GAMEROOM_SUCC			17

#define MSG_SUBID_QUERY_PLACES_END			20
#define MSG_SUBID_QUERY_ROOMS_END			21


enum GameInfoType
{
	kInfoNone	= 0,
	kGameKind	= 1,
	kGamePlace	= 2,
	kGameRoom	= 3
};

#pragma pack(push, 1)

//-------------------------------------------------------------------------------
// Common struct
//-------------------------------------------------------------------------------
struct MainValidateInfo
{
	char			szPassword[16];
	char			szServerName[64];
	char			szOS[24];
	char			szDevice[64];
	char			szLoginIp[20];

	MainValidateInfo()
	{
		::memset(szPassword, 0, sizeof(szPassword));
		::memset(szServerName, 0, sizeof(szServerName));
		::memset(szOS, 0, sizeof(szOS));
		::memset(szDevice, 0, sizeof(szDevice));
		::memset(szLoginIp, 0, sizeof(szLoginIp));
	}
};


//-------------------------------------------------------------------------------
// Net message definitions
//-------------------------------------------------------------------------------
struct CreateGuestAccountMsg
{
	GameMsgHeader		header;
	ClientStamp			clientStamp;
	MainValidateInfo	info;
};

struct ValidateLoginMainByAcctMsg
{
	GameMsgHeader		header;
	ClientStamp			clientStamp;
	char				szAccount[64];
	MainValidateInfo	info;

	ValidateLoginMainByAcctMsg()
	{
		::memset(szAccount, 0, sizeof(szAccount));
	}
};

struct ValidateLoginMainByIdMsg
{
	GameMsgHeader		header;
	ClientStamp			clientStamp;
	CSUINT32			userId;
	MainValidateInfo	info;

	ValidateLoginMainByIdMsg()
		: userId(0)
	{
	}
};

struct UserLogoutMainMsg
{
	GameMsgHeader		header;
	CSUINT32			userId;

	UserLogoutMainMsg()
		: userId(0)
	{
	}
};

struct DBAcctLoginFailMsg
{
	GameMsgHeader	header;
	ClientStamp		clientStamp;
	CSINT32			nFailReason;

	DBAcctLoginFailMsg()
		: nFailReason(0)
	{}
};

struct GetGameInfoFailMsg
{
	GameMsgHeader	header;
	CSINT32			nInfoType;
	char			szDetail[120];

	GetGameInfoFailMsg()
		: nInfoType(kInfoNone)
	{
		::memset(szDetail, 0, sizeof(szDetail));
	}
};

struct QueryGamePlaceMsg
{
	GameMsgHeader	header;
	CSINT32			nKindId;

	QueryGamePlaceMsg()
		: nKindId(-1)
	{}
};

struct QueryGameRoomMsg
{
	GameMsgHeader	header;
	CSINT32			nKindId;
	CSINT32			nPlaceId;

	QueryGameRoomMsg()
		: nKindId(-1)
		, nPlaceId(-1)
	{}
};

#pragma pack(pop)

#endif // __DB_MSG_DEFS_H__

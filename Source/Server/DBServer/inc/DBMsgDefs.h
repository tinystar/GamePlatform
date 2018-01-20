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
#define MSG_SUBID_VALIDATE_ACCOUNT_LOGIN	2
#define MSG_SUBID_VALIDATE_USERID_LOGIN		3
#define MSG_SUBID_DB_LOGIN_SUCCESS			4
#define MSG_SUBID_DB_LOGIN_FAILURE			5

#define MSG_SUBID_QUERY_GAMEKINDS			11
#define MSG_SUBID_QUERY_GAMEPLACES			12
#define MSG_SUBID_QUERY_GAMEROOMS			13
#define MSG_SUBID_GET_GAMEINFO_FAIL			14
#define	MSG_SUBID_GET_GAMEKIND_SUCC			15
#define	MSG_SUBID_GET_GAMEPLACE_SUCC		16
#define MSG_SUBID_GET_GAMEROOM_SUCC			17


enum GameInfoType
{
	kInfoNone	= 0,
	kGameKind	= 1,
	kGamePlace	= 2,
	kGameRoom	= 3
};

#pragma pack(push, 1)

struct ValidateAcctLoginMsg
{
	GameMsgHeader	header;
	ClientStamp		clientStamp;
	char			szAccount[64];
	char			szPassword[16];

	ValidateAcctLoginMsg()
	{
		::memset(szAccount, 0, sizeof(szAccount));
		::memset(szPassword, 0, sizeof(szPassword));
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

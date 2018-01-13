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

#pragma pack(pop)

#endif // __DB_MSG_DEFS_H__

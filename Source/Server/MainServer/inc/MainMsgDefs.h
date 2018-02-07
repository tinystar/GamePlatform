/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/9
/************************************************************************/
#ifndef __MAIN_MSG_DEFS_H__
#define __MAIN_MSG_DEFS_H__

#include "BaseMsgDefs.h"

#define MSG_MAINID_USER						10
#define MSG_MAINID_GAMELIST					11

// client -> server
#define MSG_SUBID_ACCOUNT_LOGIN				1
#define MSG_SUBID_WECHAT_LOGIN				2
#define MSG_SUBID_QUICK_LOGIN				3

#define MSG_SUBID_REQUEST_GAMEKINDS			1
#define MSG_SUBID_REQUEST_GAMEPLACES		2

// server -> client
#define MSG_SUBID_LOGIN_SUCCESS				1
#define MSG_SUBID_LOGIN_FAILURE				2

#define MSG_SUBID_REQ_GAMEKINDS_SUCC		1
#define MSG_SUBID_REQ_GAMEKINDS_FAIL		2
#define MSG_SUBID_REQ_GAMEPLACES_SUCC		3
#define MSG_SUBID_REQ_GAMEPLACES_FAIL		4


enum LoginFailReason
{
	eUnknownReason		= 0,
	eAccountNotExist	= 1,
	ePasswordWrong		= 2,
	eGuestCreateFail	= 3,
	eDBServerClosed		= 4,
	eAlreadyLogin		= 5
};

#pragma pack(push, 1)

struct AccountLoginMsg
{
	GameMsgHeader	header;
	char			szAccount[64];
	char			szPassword[16];

	AccountLoginMsg()
	{
		::memset(szAccount, 0, sizeof(szAccount));
		::memset(szPassword, 0, sizeof(szPassword));
	}
};

struct LoginFailMsg
{
	GameMsgHeader	header;
	CSINT32			nFailReason;

	LoginFailMsg()
		: nFailReason(0)
	{}
};

#pragma pack(pop)

#endif // __MAIN_MSG_DEFS_H__

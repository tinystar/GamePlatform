/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/8/12
/************************************************************************/
#ifndef __GATE_MSG_DEFS_H__
#define __GATE_MSG_DEFS_H__

#include "BaseMsgDefs.h"

#define MSG_MAINID_GATE						1
#define MSG_MAINID_MAIN_TO_GATE				2

// client -> server
#define MSG_SUBID_REQUEST_CONFIG			1
#define MSG_SUBID_REQUEST_MAINSVR_ADDR		2

// main -> gate
#define MSG_SUBID_MAIN_CONNECT				20

// server -> client
#define MSG_SUBID_CONFIG					1
#define MSG_SUBID_MAINSVR_ADDR				2
#define MSG_SUBID_NO_MAINSVR				3

#pragma pack(push, 1)

struct GateConfigMsg
{
	GameMsgHeader	header;
	char			szVersion[16];
	char			szUpdUrl[128];

	GateConfigMsg()
	{
		::memset(szVersion, 0, sizeof(szVersion));
		::memset(szUpdUrl, 0, sizeof(szUpdUrl));
	}
};

struct MainAddressMsg
{
	GameMsgHeader	header;
	char			szIP[20];
	CSUINT16		sPort;

	MainAddressMsg()
		: sPort(0)
	{
		::memset(szIP, 0, sizeof(szIP));
	}
};

struct MainConnectMsg
{
	GameMsgHeader	header;
	CSUINT16		sPort;
	CSUINT32		uMaxUser;

	MainConnectMsg()
		: sPort(0)
		, uMaxUser(0)
	{}
};

#pragma pack(pop)

#endif // __GATE_MSG_DEFS_H__

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

#define MSG_MAINID_GATE						150
#define MSG_MAINID_MAINSVR_DISTRI			151


#define MSG_SUBID_QUERY_CONFIG				1
#define MSG_SUBID_QUERY_MAINSVR_ADDR		2

#pragma pack(push, 1)

struct GateConfigMsg
{
	char			szVersion[16];
};

struct MainAddressMsg
{
	char			szIP[20];
	unsigned short	sPort;
};

#pragma pack(pop)

#endif // __GATE_MSG_DEFS_H__

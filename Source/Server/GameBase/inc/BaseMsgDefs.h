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

#pragma pack(pop)

#endif // __BASE_MSG_DEFS_H__

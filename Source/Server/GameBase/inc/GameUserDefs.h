/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/8/6
/************************************************************************/
#ifndef __GAME_USER_DEFS_H__
#define __GAME_USER_DEFS_H__

#include "xEzTypes.h"

class GameUser
{
public:
	GameUser();

	EzUInt32 getUserId() const { return uId; }
	bool setUserId(EzUInt32 id);

	const char* getUserName() const { return szName; }
	bool setUserName(const char* pszName);

protected:
	EzUInt32			uId;
	char				szName[256];
};

#endif // __GAME_USER_DEFS_H__

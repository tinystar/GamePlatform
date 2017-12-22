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

enum GenderType
{
	eMale	= 0,
	eFemale
};

enum UserType
{
	kRobot	= 0x00000001,
	kVip	= 0x00000002
};

class GameUser
{
public:
	GameUser();

	EzUInt32 getUserId() const { return uUserId; }
	bool setUserId(EzUInt32 id);

	const char* getUserName() const { return szName; }
	bool setUserName(const char* pszName);

	GenderType getGenderType() const { return genderType; }
	bool setGenderType(GenderType type) { genderType = type; return true; }

	double getMoney() const { return dMoney; }
	bool setMoney(double money);

	const char* getPhoneNumber() const { return szPhoneNumber; }
	bool setPhoneNumber(const char* pPhoneNum);

	bool isRobot() const { return !!(uTypeFlag & kRobot); }
	bool setRobot(bool bRobot);

	bool isVip() const { return !!(uTypeFlag & kVip); }
	bool setVip(bool bVip);

protected:
	EzUInt32			uUserId;
	char				szName[256];
	GenderType			genderType;
	double				dMoney;
	char				szPhoneNumber[16];
	EzUInt32			uTypeFlag;
};

#endif // __GAME_USER_DEFS_H__

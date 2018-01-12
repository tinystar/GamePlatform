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
#include "BaseMsgDefs.h"
#include "GameBase.h"

enum GenderType
{
	eMale	= 0,
	eFemale
};

enum UserType
{
	kRobot	= 0x00000001,
	kVip	= 0x00000002,
	kGuest	= 0x00000004
};

class GB_DLL_SPEC GameUser
{
public:
	GameUser();
	explicit GameUser(const UserInfo& userInfo);

	void initWithUserInfo(const UserInfo& userInfo);

	EzUInt32 getUserId() const { return m_uUserId; }
	bool setUserId(EzUInt32 id);

	const char* getAccount() const { return m_szAccount; }
	bool setAccount(const char* pszAccount);

	const char* getUserName() const { return m_szName; }
	bool setUserName(const char* pszName);

	GenderType getGenderType() const { return m_genderType; }
	bool setGenderType(GenderType type) { m_genderType = type; return true; }

	double getMoney() const { return m_dMoney; }
	bool setMoney(double money);

	EzUInt32 getRoomCardCount() const { return m_uRoomCardCount; }
	bool setRoomCardCount(EzUInt32 count);

	const char* getPhoneNumber() const { return m_szPhoneNumber; }
	bool setPhoneNumber(const char* pPhoneNum);

	bool isRobot() const { return !!(m_uTypeFlag & kRobot); }
	bool setRobot(bool bRobot);

	bool isVip() const { return !!(m_uTypeFlag & kVip); }
	bool setVip(bool bVip);

	bool isGuest() const { return !!(m_uTypeFlag & kGuest); }
	bool setGuest(bool bGuest);

protected:
	EzUInt32			m_uUserId;
	char				m_szAccount[17];
	char				m_szName[65];
	GenderType			m_genderType;
	double				m_dMoney;
	EzUInt32			m_uRoomCardCount;
	char				m_szPhoneNumber[16];
	EzUInt32			m_uTypeFlag;
};

#endif // __GAME_USER_DEFS_H__

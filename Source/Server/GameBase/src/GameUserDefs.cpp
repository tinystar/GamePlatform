#include "GameUserDefs.h"
#include "xEzUtil.h"
#include <memory.h>
#include <string.h>


GameUser::GameUser()
	: m_uUserId(0)
	, m_genderType(eMale)
	, m_dMoney(0)
	, m_uTypeFlag(0)
{
	::memset(m_szAccount, 0, sizeof(m_szAccount));
	::memset(m_szName, 0, sizeof(m_szName));
	::memset(m_szPhoneNumber, 0, sizeof(m_szPhoneNumber));
}

GameUser::GameUser(const UserInfo& userInfo)
	: m_uUserId(0)
	, m_genderType(eMale)
	, m_dMoney(0)
	, m_uTypeFlag(0)
{
	::memset(m_szAccount, 0, sizeof(m_szAccount));
	::memset(m_szName, 0, sizeof(m_szName));
	::memset(m_szPhoneNumber, 0, sizeof(m_szPhoneNumber));

	initWithUserInfo(userInfo);
}

void GameUser::initWithUserInfo(const UserInfo& userInfo)
{
	setUserId(userInfo.userId);
	setAccount(userInfo.szAccount);
	setUserName(userInfo.szUserName);
	setGenderType((GenderType)userInfo.genderType);
	setMoney(userInfo.uMoney / 100.0);
	setRoomCardCount(userInfo.uRoomCard);
	setPhoneNumber(userInfo.szPhoneNum);
	m_uTypeFlag = userInfo.uTypeFlag;
}

bool GameUser::setUserId(EzUInt32 id)
{
	if (!EzVerify(id > 0))
		return false;

	m_uUserId = id;
	return true;
}

bool GameUser::setAccount(const char* pszAccount)
{
	if (NULL == pszAccount || 0 == *pszAccount)
		return false;
	if (strlen(pszAccount) > (sizeof(m_szAccount) - 1))
		return false;

	strcpy(m_szAccount, pszAccount);
	return true;
}

bool GameUser::setUserName(const char* pszName)
{
	if (NULL == pszName || 0 == *pszName)
		return false;
	if (strlen(pszName) > (sizeof(m_szName) - 1))
		return false;

	strcpy(m_szName, pszName);
	return true;
}

bool GameUser::setMoney(double money)
{
	m_dMoney = money;
	return true;
}

bool GameUser::setRoomCardCount(EzUInt32 count)
{
	m_uRoomCardCount = count;
	return true;
}

bool GameUser::setPhoneNumber(const char* pPhoneNum)
{
	if (NULL == pPhoneNum)
		pPhoneNum = "";
	if (strlen(pPhoneNum) > (sizeof(m_szPhoneNumber) - 1))
		return false;

	strcpy(m_szPhoneNumber, pPhoneNum);
	return true;
}

bool GameUser::setRobot(bool bRobot)
{
	if (bRobot)
		m_uTypeFlag |= kRobot;
	else
		m_uTypeFlag &= ~kRobot;

	return true;
}

bool GameUser::setVip(bool bVip)
{
	if (bVip)
		m_uTypeFlag |= kVip;
	else
		m_uTypeFlag &= ~kVip;

	return true;
}

bool GameUser::setGuest(bool bGuest)
{
	if (bGuest)
		m_uTypeFlag |= kGuest;
	else
		m_uTypeFlag &= ~kGuest;

	return true;
}

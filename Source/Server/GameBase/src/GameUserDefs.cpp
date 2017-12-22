#include "GameUserDefs.h"
#include "xEzUtil.h"
#include <memory.h>
#include <string.h>


GameUser::GameUser()
	: uUserId(0)
	, genderType(eMale)
	, dMoney(0)
	, uTypeFlag(0)
{
	::memset(szName, 0, sizeof(szName));
	::memset(szPhoneNumber, 0, sizeof(szPhoneNumber));
}

bool GameUser::setUserId(EzUInt32 id)
{
	if (!EzVerify(id > 0))
		return false;

	uUserId = id;
	return true;
}

bool GameUser::setUserName(const char* pszName)
{
	if (NULL == pszName || 0 == *pszName)
		return false;
	if (strlen(pszName) > 255)
		return false;

	strcpy(szName, pszName);
	return true;
}

bool GameUser::setMoney(double money)
{
	dMoney = money;
	return true;
}

bool GameUser::setPhoneNumber(const char* pPhoneNum)
{
	if (NULL == pPhoneNum)
		pPhoneNum = "";
	if (strlen(pPhoneNum) > 15)
		return false;

	strcpy(szPhoneNumber, pPhoneNum);
	return true;
}

bool GameUser::setRobot(bool bRobot)
{
	if (bRobot)
		uTypeFlag |= kRobot;
	else
		uTypeFlag &= ~kRobot;

	return true;
}

bool GameUser::setVip(bool bVip)
{
	if (bVip)
		uTypeFlag |= kVip;
	else
		uTypeFlag &= ~kVip;

	return true;
}

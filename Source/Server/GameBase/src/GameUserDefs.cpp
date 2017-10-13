#include "GameUserDefs.h"
#include <memory.h>
#include <string.h>


GameUser::GameUser()
	: uId(0)
{
	::memset(szName, 0, sizeof(szName));
}

bool GameUser::setUserId(EzUInt32 id)
{
	uId = id;
	return true;
}

bool GameUser::setUserName(const char* pszName)
{
	if (strlen(pszName) > 255)
		return false;

	strcpy(szName, pszName);
	return true;
}


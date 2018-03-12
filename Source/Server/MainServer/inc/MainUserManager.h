/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/3/11
/************************************************************************/
#ifndef __MAIN_USER_MANAGER_H__
#define __MAIN_USER_MANAGER_H__

#include "ClientId.h"
#include "BaseMsgDefs.h"
#include <map>

class GameUser;

class MainUserManager
{
public:
	MainUserManager();
	~MainUserManager();

public:
	bool addLoginUser(ClientId cId, const UserInfo& userInfo);
	bool removeLoginUser(ClientId cId);

	ClientId findClientIdByUserId(EzUInt32 userId) const;

	GameUser* getGameUserByClientId(ClientId cId) const;

	void cleanupAllUser();

protected:
	typedef std::map<EzUInt32, ClientId>	UserId2ClientIdMap;

	UserId2ClientIdMap		m_userIdToClientMap;
};

#endif // __MAIN_USER_MANAGER_H__

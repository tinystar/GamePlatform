/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/4/6
/************************************************************************/
#ifndef __ROOM_PLAYER_MANAGER_H__
#define __ROOM_PLAYER_MANAGER_H__

#include "ClientId.h"
#include <map>

class RoomPlayer;

class RoomPlayerManager
{
public:
	RoomPlayerManager();
	~RoomPlayerManager();

public:
	bool addLoginPlayer(ClientId cId);
	bool removeLoginPlayer(ClientId cId);

	size_t getLoginCount() const { return m_onlinePlayers.size(); }

	RoomPlayer* getRoomPlayerByClientId(ClientId cId) const;

	void cleanupAllPlayer();

protected:
	typedef std::map<EzUInt32, ClientId>	UserId2ClientIdMap;

	UserId2ClientIdMap		m_onlinePlayers;
};

#endif // __ROOM_PLAYER_MANAGER_H__

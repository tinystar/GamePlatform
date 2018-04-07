/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/4/6
/************************************************************************/
#ifndef __ROOM_PLAYER_H__
#define __ROOM_PLAYER_H__

#include "GameUserDefs.h"

const int INVALID_DESK_NUMBER = -1;
const int INVALID_CHAIR_POS   = -1;

enum PlayerState
{
	kStateNone = 0,
	kStateLook,							// Player has logged in room but not sit down to the desk.
	kStateSit,
	kStateAgree,
	kStatePlay
};

class RoomPlayer : GameUser
{
public:
	RoomPlayer();

	int getDeskNumber() const { return m_nDeskNumber; }
	int getChairPos() const { return m_nChairPos; }

	PlayerState getPlayerState() const { return m_playerState; }

protected:
	int				m_nDeskNumber;
	int				m_nChairPos;
	PlayerState		m_playerState;
};

#endif // __ROOM_PLAYER_H__

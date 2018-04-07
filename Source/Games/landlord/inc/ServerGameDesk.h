/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/2/25
/************************************************************************/
#ifndef __SERVER_GAME_DESK_H__
#define __SERVER_GAME_DESK_H__

#include "GameDesk.h"

const int DESK_CHAIR_COUNT = 3;

class ServerGameDesk : public GameDesk
{
public:
	ServerGameDesk();
	virtual ~ServerGameDesk();
};

#endif // __SERVER_GAME_DESK_H__

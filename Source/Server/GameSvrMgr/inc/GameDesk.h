/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/11/26
/************************************************************************/
#ifndef __GAME_DESK_H__
#define __GAME_DESK_H__

#include "GameSvrMgr.h"
#include "xEzUtil.h"

class GSM_DLL_SPEC GameDesk
{
protected:
	GameDesk(int nChairs);

public:
	virtual ~GameDesk();

public:
	int getChairCount() const { return m_nChairCount; }

protected:
	int				m_nChairCount;
};

class GameDeskFactory
{
public:
	virtual GameDesk* createGameDesk() = 0;
	virtual void destroyGameDesk(GameDesk*& pDesk) = 0;
};

template<class GameDeskClass>
class GameDeskFactoryTemplate : public GameDeskFactory
{
	virtual GameDesk* createGameDesk()
	{
		return new GameDeskClass();
	}

	virtual void destroyGameDesk(GameDesk*& pDesk)
	{
		delete pDesk;
		pDesk = NULL;
	}
};

#endif // __GAME_DESK_H__

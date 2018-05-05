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
#include "RoomPlayer.h"

class GSM_DLL_SPEC GameDesk
{
	friend class GameRoomServer;
	friend class DeskTimerTask;

protected:
	explicit GameDesk(int nChairs);

public:
	virtual ~GameDesk();

public:
	int getChairCount() const { return m_nChairCount; }
	int getDeskNumber() const { return m_nDeskNumber; }

	// These functions is used by GameRoomServer.
protected:
	bool init(GameRoomServer* pRoomServer, int nNumber);

protected:
	// The underlying implementation is ticked every 100ms. So, the minimum resolution is 100ms.
	// `uElapse` is better to be a multiplier of 100.
	bool setTimer(EzUInt uTimerId, EzUInt uElapse);
	bool killTimer(EzUInt uTimerId);

protected:
	virtual void onTimer(EzUInt uTimerId);

private:
	GameRoomServer*	m_pRoomServer;

protected:
	int				m_nChairCount;
	int				m_nDeskNumber;
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

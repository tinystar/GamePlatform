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


class GameDesk
{

};

class GameDeskFactory
{
	virtual GameDesk* createGameDesk() = 0;
	virtual GameDesk* createGameDesk(int nDeskCount) = 0;
};

template<class GameDeskClass>
class GameDeskFactoryTemplate : public GameDeskFactory
{
	virtual GameDesk* createGameDesk()
	{
		return new GameDeskClass();
	}

	virtual GameDesk* createGameDesk(int nDeskCount)
	{
		return new GameDeskClass[nDeskCount];
	}
};

#endif // __GAME_DESK_H__

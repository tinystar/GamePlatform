/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/5/4
/************************************************************************/
#ifndef __DESK_TIMERS_MANAGER_H__
#define __DESK_TIMERS_MANAGER_H__

#include "TimerTask.h"
#include "TimerTaskManager.h"
#include <map>

class GameDesk;

class DeskTimerTask : public TimerTask
{
public:
	DeskTimerTask(GameDesk* pDesk, EzUInt uTimerId, EzUInt uElapse);

	void kill() { m_bKilled = true; }

protected:
	virtual ContinueMode run(TimerTaskManager* pTaskMgr);

	virtual unsigned int reset() { m_bKilled = false; return m_uElapse; }

private:
	GameDesk*	m_pGameDesk;
	EzUInt		m_uTimerId;
	EzUInt		m_uElapse;
	bool		m_bKilled;
};

class DeskTimerKey
{
public:
	DeskTimerKey(GameDesk* pDesk, EzUInt uTimerId)
		: m_pGameDesk(pDesk)
		, m_uTimerId(uTimerId)
	{}

	bool operator< (const DeskTimerKey& rhs) const
	{
		if (m_pGameDesk < rhs.m_pGameDesk)
			return true;
		else if (m_pGameDesk == rhs.m_pGameDesk)
			return m_uTimerId < rhs.m_uTimerId;

		return false;
	}

private:
	GameDesk*	m_pGameDesk;
	EzUInt		m_uTimerId;
};

class DeskTimersManager
{
public:
	DeskTimersManager();
	~DeskTimersManager();

public:
	bool setDeskTimer(GameDesk* pDesk, EzUInt uTimerId, EzUInt uElapse);
	bool killDeskTimer(GameDesk* pDesk, EzUInt uTimerId);

	void clear();

	EzUInt updateInterval() const;
	void update();

protected:
	typedef std::map<DeskTimerKey, DeskTimerTask*> DeskTimerIdToTaskMap;

	TimerTaskManager		m_DeskTimerTaskMgr;
	DeskTimerIdToTaskMap	m_GameDeskTimers;
};

#endif // __DESK_TIMERS_MANAGER_H__

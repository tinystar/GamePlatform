#include "DeskTimersManager.h"
#include "GameDesk.h"

const unsigned int TIMER_TICK_DURATION = 100;		// ms
const unsigned int TIMER_TICKS_PER_WHEEL = 1024;

//------------------------------------------------------
// DeskTimerTask
//------------------------------------------------------
DeskTimerTask::DeskTimerTask(GameDesk* pDesk, EzUInt uTimerId, EzUInt uElapse)
	: m_pGameDesk(pDesk)
	, m_uTimerId(uTimerId)
	, m_uElapse(uElapse)
	, m_bKilled(false)
{
}

TimerTask::ContinueMode DeskTimerTask::run(TimerTaskManager* pTaskMgr)
{
	if (m_bKilled)
		return kKill;

	m_pGameDesk->onTimer(m_uTimerId);
	// Kill this task if user kill the timer in the onTimer notification function.
	return m_bKilled ? kKill : kContinue;
}


//------------------------------------------------------
// DeskTimersManager
//------------------------------------------------------
DeskTimersManager::DeskTimersManager()
	: m_DeskTimerTaskMgr(TIMER_TICK_DURATION, TIMER_TICKS_PER_WHEEL)
{

}

DeskTimersManager::~DeskTimersManager()
{

}

bool DeskTimersManager::setDeskTimer(GameDesk* pDesk, EzUInt uTimerId, EzUInt uElapse)
{
	DeskTimerIdToTaskMap::iterator iter = m_GameDeskTimers.find(DeskTimerKey(pDesk, uTimerId));
	if (iter != m_GameDeskTimers.end())
	{
		if (!m_DeskTimerTaskMgr.resetTask(iter->second, uElapse))
		{
			m_GameDeskTimers.erase(iter);
			return false;
		}

		return true;
	}
	else
	{
		DeskTimerTask* pTask = new DeskTimerTask(pDesk, uTimerId, uElapse);
		if (m_DeskTimerTaskMgr.addNewTask(pTask, uElapse))
		{
			m_GameDeskTimers.insert(std::make_pair(DeskTimerKey(pDesk, uTimerId), pTask));
			return true;
		}

		return false;
	}
}

bool DeskTimersManager::killDeskTimer(GameDesk* pDesk, EzUInt uTimerId)
{
	DeskTimerIdToTaskMap::iterator iter = m_GameDeskTimers.find(DeskTimerKey(pDesk, uTimerId));
	if (iter != m_GameDeskTimers.end())
	{
		iter->second->kill();				// Set killed flags only, otherwise the task may be killed during processing the task.
		m_GameDeskTimers.erase(iter);
		return true;
	}

	return false;
}

void DeskTimersManager::clear()
{
	m_GameDeskTimers.clear();
}

EzUInt DeskTimersManager::updateInterval() const
{
	return TIMER_TICK_DURATION;
}

void DeskTimersManager::update()
{
	m_DeskTimerTaskMgr.tick();
}

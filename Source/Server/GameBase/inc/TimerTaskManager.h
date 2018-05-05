/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/4/11
/************************************************************************/
#ifndef __TIMER_TASK_MANAGER_H__
#define __TIMER_TASK_MANAGER_H__

#include "TimerTask.h"
#include "GameBase.h"

// 借鉴时间轮算法用一个定时器来管理大量的定时任务，目前的实现依赖底层Timer
// 的精度，如果精度太低，误差会被累积放大
class GB_DLL_SPEC TimerTaskManager
{
EZ_FORBID_CLASS_COPY(TimerTaskManager)

public:
	TimerTaskManager(unsigned int uTickDuration, unsigned int uTicksPerWheel);
	~TimerTaskManager();

public:
	// TimerTask must be allocated on the heap and it's memory will be managed 
	// by the manager when it's be successfully added to the manager.
	bool addNewTask(TimerTask* pTask, unsigned int uDelay);

	// The task will be removed from the manager and it's memory will be released.
	bool killTask(TimerTask* pTask);

	bool resetTask(TimerTask* pTask, unsigned int uNewDelay);

	void tick();

protected:
	unsigned int normalizeTicksPerWheel(unsigned int uTicksPerWheel);
	void createWheel();
	void destroyWheel();

	bool removeTask(TimerTask* pTask);

protected:
	unsigned int		m_uTickDuration;
	unsigned int		m_uTicksPerWheel;
	unsigned int		m_uMask;
	TaskNode*			m_wheel;
	unsigned int		m_uCurTick;
};

#endif // __TIMER_TASK_MANAGER_H__

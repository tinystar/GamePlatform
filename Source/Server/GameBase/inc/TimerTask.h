/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/4/11
/************************************************************************/
#ifndef __TIMER_TASK_H__
#define __TIMER_TASK_H__

class TaskNode
{
	friend class TimerTaskManager;

protected:
	TaskNode();
	~TaskNode();

private:
	TaskNode*		m_pPrev;
	TaskNode*		m_pNext;
	unsigned int	m_uRound;			// remaining round
};

class TimerTask : public TaskNode
{
	friend class TimerTaskManager;

	// The memory is only managed by the TimerTaskManager after allocated
protected:
	virtual ~TimerTask();

public:
	enum ContinueMode
	{
		kKill = 0,			// the task will be deleted
		kContinue
	};

	virtual ContinueMode run(TimerTaskManager* pTaskMgr) = 0;

	// Return the delay time that the task next expired if kContinue returned in the run method. 
	virtual unsigned int reset() { return 0; }
};

#endif // __TIMER_TASK_H__

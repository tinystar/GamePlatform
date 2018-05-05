#include "TimerTaskManager.h"
#include "xEzUtil.h"


TimerTaskManager::TimerTaskManager(unsigned int uTickDuration, unsigned int uTicksPerWheel)
	: m_uTickDuration(uTickDuration)
	, m_uTicksPerWheel(uTicksPerWheel)
	, m_uMask(0)
	, m_wheel(NULL)
	, m_uCurTick(0)
{
	if (0 == m_uTickDuration || 0 == m_uTicksPerWheel)
		throw EzException(_T("Tick duration or  ticks per wheel can't be NULL!"), _T(__FILE__), __LINE__);

	createWheel();
}

TimerTaskManager::~TimerTaskManager()
{
	destroyWheel();
}

bool TimerTaskManager::addNewTask(TimerTask* pTask, unsigned int uDelay)
{
	if (NULL == pTask)
		return false;

	// already added
	if (pTask->m_pPrev != NULL || pTask->m_pNext != NULL)
		return false;

	if (0 == uDelay)
	{
		delete pTask;
		pTask = NULL;
		return false;
	}

	// 计算需要多少个tick
	unsigned int uNeedTicks = (uDelay + m_uTickDuration - 1) / m_uTickDuration;		// 以m_uTickDuration为间距向上取整进一位

	pTask->m_uRound = uNeedTicks / m_uTicksPerWheel;								// 计算任务需要几圈
	uNeedTicks &= m_uMask;															// 等价于uNeedTicks % m_uTicksPerWheel
	unsigned int uIdx = (m_uCurTick + uNeedTicks) & m_uMask;

	TaskNode& bucket = m_wheel[uIdx];
	if (bucket.m_pPrev == NULL && bucket.m_pNext == NULL)
	{
		bucket.m_pNext = pTask;
		pTask->m_pPrev = &bucket;
		pTask->m_pNext = &bucket;
		bucket.m_pPrev = pTask;
	}
	else
	{
		bucket.m_pPrev->m_pNext = pTask;
		pTask->m_pPrev = bucket.m_pPrev;
		pTask->m_pNext = &bucket;
		bucket.m_pPrev = pTask;
	}

	return true;
}

bool TimerTaskManager::killTask(TimerTask* pTask)
{
	if (removeTask(pTask))
	{
		delete pTask;
		pTask = NULL;
		return true;
	}

	return false;
}

bool TimerTaskManager::resetTask(TimerTask* pTask, unsigned int uNewDelay)
{
	if (!removeTask(pTask))
		return false;

	return addNewTask(pTask, uNewDelay);
}

void TimerTaskManager::tick()
{
	++m_uCurTick;
	m_uCurTick &= m_uMask;				// % m_uTicksPerWheel

	TaskNode* pCurNode = m_wheel[m_uCurTick].m_pNext;
	while (pCurNode != NULL && pCurNode != &m_wheel[m_uCurTick])	// not empty and not reached the head node
	{
		TaskNode* pNext = pCurNode->m_pNext;
		if (0 == pCurNode->m_uRound)
		{
			TimerTask* pCurTask = (TimerTask*)pCurNode;
			// 先移除掉避免在run函数中调用killTask引起崩溃
			removeTask(pCurTask);
			TimerTask::ContinueMode mode = pCurTask->run(this);
			if (TimerTask::kContinue == mode)
				addNewTask(pCurTask, pCurTask->reset());
			else
				delete pCurTask;
		}
		else
		{
			--pCurNode->m_uRound;
		}

		pCurNode = pNext;
	}
}

unsigned int TimerTaskManager::normalizeTicksPerWheel(unsigned int uTicksPerWheel)
{
	unsigned int uResult = 1;
	while (uResult < uTicksPerWheel)
		uResult <<= 1;

	return uResult;
}

void TimerTaskManager::createWheel()
{
	m_uTicksPerWheel = normalizeTicksPerWheel(m_uTicksPerWheel);
	m_wheel = new TaskNode[m_uTicksPerWheel];
	if (NULL == m_wheel)
		throw EzException(_T("Allocate wheel failed, memory may not enough!"), _T(__FILE__), __LINE__);

	m_uMask = m_uTicksPerWheel - 1;
}

void TimerTaskManager::destroyWheel()
{
	for (unsigned int i = 0; i < m_uTicksPerWheel; ++i)
	{
		TaskNode* pCurTask = m_wheel[i].m_pNext;
		while (pCurTask != NULL && pCurTask != &m_wheel[i])	// not empty and not reached the head node
		{
			TaskNode* pNext = pCurTask->m_pNext;
			delete (TimerTask*)pCurTask;		// must cast to TimerTask type
			pCurTask = pNext;
		}
	}

	EZ_SAFE_DELETE_ARRAY(m_wheel);
}

bool TimerTaskManager::removeTask(TimerTask* pTask)
{
	if (NULL == pTask)
		return false;

	// new task
	if (NULL == pTask->m_pPrev && NULL == pTask->m_pNext)
		return false;

	if (pTask->m_pPrev == pTask->m_pNext)
	{
		pTask->m_pPrev->m_pNext = NULL;
		pTask->m_pNext->m_pPrev = NULL;
	}
	else
	{
		pTask->m_pPrev->m_pNext = pTask->m_pNext;
		pTask->m_pNext->m_pPrev = pTask->m_pPrev;
	}

	pTask->m_pPrev = pTask->m_pNext = NULL;
	return true;
}

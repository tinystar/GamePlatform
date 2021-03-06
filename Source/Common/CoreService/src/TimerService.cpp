#include "TimerService.h"
#ifdef EZ_WINDOWS
#include "win/TimerServiceImp.h"
#elif defined(EZ_LINUX)
// todo
#endif
#include "MiniHeapTimer.h"


//-------------------------------------------------------------------------------
// TimerService
//-------------------------------------------------------------------------------
TimerService::TimerService()
{
}

TimerService::~TimerService()
{
}

bool TimerService::addEventHandler(ITimerServiceEventHandler* pEventHandler)
{
	if (NULL == pEventHandler)
		return false;

	int idx = m_EventHandlers.find(pEventHandler);
	if (idx < 0)
		m_EventHandlers.append(pEventHandler);
	return true;
}

bool TimerService::removeEventHandler(ITimerServiceEventHandler* pEventHandler)
{
	int idx = m_EventHandlers.find(pEventHandler);
	if (idx < 0)
		return false;

	m_EventHandlers.removeAt(idx);
	return true;
}


//-------------------------------------------------------------------------------
// create/release timer service
//-------------------------------------------------------------------------------
TimerService* createTimerService(TimerType type /*= kMiniHeap*/)
{
	TimerService* pService = NULL;
	switch (type)
	{
	case kSysTimer:
		pService = new TimerServiceImp();
		break;
	case kMiniHeap:
		pService = new MiniHeapTimer();
		break;
	default:
		EzAssert(false);
	}

	return pService;
}

void releaseTimerService(TimerService* pService)
{
	if (pService)
		delete pService;
}

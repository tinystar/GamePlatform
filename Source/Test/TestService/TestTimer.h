#ifndef __TEST_TIMER_H__
#define __TEST_TIMER_H__

#include "TimerService.h"

class TestTimer : public ITimerServiceEventHandler
{
public:
	TestTimer();
	~TestTimer();

	bool start();
	bool stop();

	bool setTimer(EzUInt uTimerId, EzUInt uElapse);

public:
	virtual void onTimerMessage(EzUInt uTimerId);

protected:
	TimerService*			m_pTimerService;
	bool					m_bRunning;
};

#endif
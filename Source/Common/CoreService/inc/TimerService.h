/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/3/19
/************************************************************************/
#ifndef __TIMER_SERVICE_H__
#define __TIMER_SERVICE_H__

#include "CoreService.h"

// Timer event notify interface
struct ITimerServiceEventHandler
{
	virtual void onTimerMessage(EzUInt uTimerId) = 0;
};

class TimerService
{
EZ_FORBID_CLASS_COPY(TimerService)

public:
	TimerService();
	virtual ~TimerService();

public:
	virtual SVCErrorCode init() = 0;
	virtual SVCErrorCode unInit() = 0;
	virtual SVCErrorCode start() = 0;
	virtual SVCErrorCode stop() = 0;

	virtual bool setTimer(EzUInt uTimerId, EzUInt uElapse) = 0;

	virtual bool killTimer(EzUInt uTimerId) = 0;

	// for debug
	virtual void dump(EzDumpContext& dc) const = 0;

public:
	bool addEventHandler(ITimerServiceEventHandler* pEventHandler);
	bool removeEventHandler(ITimerServiceEventHandler* pEventHandler);

protected:
	typedef EzArray<ITimerServiceEventHandler*>	EventHandlerArray;

	EventHandlerArray			m_EventHandlers;
};

SVC_DLL_SPEC TimerService* createTimerService();
SVC_DLL_SPEC void releaseTimerService(TimerService* pService);

#endif // __TIMER_SERVICE_H__
/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/4/22
/************************************************************************/
#ifndef __MINI_HEAP_TIMER_H__
#define __MINI_HEAP_TIMER_H__

#include "TimerService.h"
#include <map>
#include "TimerHeap.h"

class MiniHeapTimer : public TimerService
{
public:
	MiniHeapTimer();
	virtual ~MiniHeapTimer();

	// TimerService interfaces
public:
	virtual SVCErrorCode init();
	virtual SVCErrorCode unInit();
	virtual SVCErrorCode start();
	virtual SVCErrorCode stop();

	virtual bool setTimer(EzUInt uTimerId, EzUInt uElapse);

	virtual bool killTimer(EzUInt uTimerId);

	virtual void dump(EzDumpContext& dc) const;

protected:
	static unsigned __stdcall timerThreadProc(void* pParam);

	EzInt64 getNowElapsedTime() const;
	EzInt64 calcTimerExpiresTime(EzUInt uElapse) const;

	void notifyTimerMsg(EzUInt uTimerId);

	void timerThreadInit();
	void timerThreadRun();

	static void CALLBACK timerAPCProc(ULONG_PTR dwParam);

	bool addTimerToHeap(EzUInt uTimerId, EzUInt uElapse);
	bool removeTimerFromHeap(EzUInt uTimerId);

private:
	enum eStateFlags
	{
		kInited		= 0x00000001,
		kRunning	= 0x00000002
	};

	EzUInt32				m_stateFlags;
	HANDLE					m_hTimerThread;
	EzInt64					m_nStartCounter;
	EzInt64					m_nFrequency;
	TimerHeap				m_timerHeap;
	volatile bool			m_bQuit;
};

#endif // __MINI_HEAP_TIMER_H__
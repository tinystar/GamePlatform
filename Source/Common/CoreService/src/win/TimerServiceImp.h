/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/3/19
/************************************************************************/
#ifndef __TIMER_SERVICE_IMP_H__
#define __TIMER_SERVICE_IMP_H__

#include "TimerService.h"

class TimerServiceImp : public TimerService
{
public:
	TimerServiceImp();
	virtual ~TimerServiceImp();

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

	static LRESULT CALLBACK timerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	void notifyTimerMsg(EzUInt uTimerId);

private:
	enum eStateFlags
	{
		kInited		= 0x00000001,
		kRunning	= 0x00000002
	};

	EzUInt32			m_stateFlags;
	HWND				m_hMsgWnd;
	HANDLE				m_hThreadEvent;
};

#endif // __TIMER_SERVICE_IMP_H__
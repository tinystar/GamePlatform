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

	virtual bool setTimer(unsigned int uTimerId, unsigned int uElapse);

	virtual bool killTimer(unsigned int uTimerId);

	virtual void dump(EzDumpContext& dc) const;
};

#endif // __TIMER_SERVICE_IMP_H__
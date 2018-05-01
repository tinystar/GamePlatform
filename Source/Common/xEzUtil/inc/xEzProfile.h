/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
//* Date: 2017/1/6
/************************************************************************/
#ifndef __XEZ_PROFILE_H__
#define __XEZ_PROFILE_H__

#include "xEzCompile.h"

class EzStopwatchImp;

class EZ_DLL_SPEC EzStopwatch
{
public:
	EzStopwatch();
	~EzStopwatch();

	void start() const;

	// Returns the time in seconds passed from the start() function been called.
	double stop() const;

private:
	EzStopwatchImp*	m_pImpWatch;
};

#endif // __XEZ_PROFILE_H__
/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/12/4
/************************************************************************/
#ifndef __XEZ_TIME_H__
#define __XEZ_TIME_H__

#include "xEzCompile.h"
#include "xEzMacro.h"
#include <time.h>

EZ_NAMESPACE_BEGIN(EzTime)

EZ_DLL_C_SPEC bool getDateToday(int& year, int& month, int& day);
EZ_DLL_C_SPEC bool getDateFromTime(time_t tm, int& year, int& month, int& day);
EZ_DLL_C_SPEC bool getTimeCurrent(int& hour, int& min, int& sec);

EZ_NAMESPACE_END

#endif // __XEZ_TIME_H__
/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/11/27
/************************************************************************/
#ifndef __XEZ_DEBUG_H__
#define __XEZ_DEBUG_H__

#include <assert.h>
#include <tchar.h>
#include "xEzCompile.h"

EZ_DLL_C_SPEC void MyTrace(const TCHAR* fmt, ...);

#if defined(_DEBUG) || defined(DEBUG)
	#define EzAssert(expr)	( (!!(expr)) || (_wassert(_CRT_WIDE(#expr), _CRT_WIDE(__FILE__), __LINE__), 0) )
	#define EzVerify(expr)	EzAssert(expr)
#else
	#define EzAssert(expr)
	#define EzVerify(expr)	(expr)
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define EzTrace(fmt, ...)								\
	do													\
	{													\
		MyTrace(_T("[TRACE]%s: "), _T(__FUNCTION__));	\
		MyTrace(fmt, ##__VA_ARGS__);					\
		MyTrace(_T("\n"));								\
	} while(0)
#else
#define EzTrace				__noop
#endif

#endif // __XEZ_DEBUG_H__
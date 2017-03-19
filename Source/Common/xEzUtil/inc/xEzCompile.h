/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/11/27
/************************************************************************/
#ifndef __XEZ_COMPILE_H__
#define __XEZ_COMPILE_H__

#include "xEzPlatform.h"

#ifdef __cplusplus
	#define EZ_C_EXTERNS extern "C"
#else
	#define EZ_C_EXTERNS
#endif

#ifdef EZ_WINDOWS
	#ifdef XEZ_UTIL_STATIC
		#define EZ_DLL_SPEC
		#define EZ_DLL_C_SPEC EZ_C_EXTERNS
	#else	// XEZ_UTIL_STATIC
		#ifdef XEZ_UTIL_EXPORTS
			#define EZ_DLL_SPEC __declspec(dllexport)
			#define EZ_DLL_C_SPEC EZ_C_EXTERNS __declspec(dllexport)
		#else
			#define EZ_DLL_SPEC __declspec(dllimport)
			#define EZ_DLL_C_SPEC EZ_C_EXTERNS __declspec(dllimport)
		#endif
	#endif	// XEZ_UTIL_STATIC
#else	// EZ_WINDOWS
	#define EZ_DLL_SPEC
	#define EZ_DLL_C_SPEC EZ_C_EXTERNS
#endif


#if defined(_MSC_VER)
	#define EZFORCEINLINE	__forceinline
	#define EZSTDCALL		__stdcall
#else
	#define EZFORCEINLINE	inline
	#define EZSTDCALL
#endif

#endif // __XEZ_COMPILE_H__
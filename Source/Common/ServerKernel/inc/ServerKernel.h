/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/3/20
/************************************************************************/
#ifndef __SERVER_KERNEL_H__
#define __SERVER_KERNEL_H__

#include "xEzUtil.h"

#ifdef __cplusplus
#define SK_C_EXTERNS extern "C"
#else
#define SK_C_EXTERNS
#endif

#ifdef EZ_WINDOWS
#ifdef SERVER_KERNEL_EXPORTS
#define SK_DLL_SPEC __declspec(dllexport)
#define SK_DLL_C_SPEC SK_C_EXTERNS __declspec(dllexport)
#else
#define SK_DLL_SPEC __declspec(dllimport)
#define SK_DLL_C_SPEC SK_C_EXTERNS __declspec(dllimport)
#endif
#else	// EZ_WINDOWS
#define SK_DLL_SPEC
#define SK_DLL_C_SPEC
#endif

#endif // __SERVER_KERNEL_H__
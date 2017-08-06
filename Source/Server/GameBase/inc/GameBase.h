/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/7/31
/************************************************************************/
#ifndef __GAME_BASE_H__
#define __GAME_BASE_H__

#include "ServerKernel.h"

#ifdef __cplusplus
#define GB_C_EXTERNS extern "C"
#else
#define GB_C_EXTERNS
#endif

#ifdef EZ_WINDOWS
#ifdef GAME_BASE_EXPORTS
#define GB_DLL_SPEC __declspec(dllexport)
#define GB_DLL_C_SPEC GB_C_EXTERNS __declspec(dllexport)
#else
#define GB_DLL_SPEC __declspec(dllimport)
#define GB_DLL_C_SPEC GB_C_EXTERNS __declspec(dllimport)
#endif
#else	// EZ_WINDOWS
#define GB_DLL_SPEC
#define GB_DLL_C_SPEC
#endif

#endif // __GAME_BASE_H__

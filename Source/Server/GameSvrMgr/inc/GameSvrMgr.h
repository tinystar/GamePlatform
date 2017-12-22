/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/11/26
/************************************************************************/
#ifndef __GAME_SVR_MGR_H__
#define __GAME_SVR_MGR_H__

#include "xEzUtil.h"

#ifdef __cplusplus
#define GSM_C_EXTERNS extern "C"
#else
#define GSM_C_EXTERNS
#endif

#ifdef EZ_WINDOWS
#ifdef GAME_SVRMGR_EXPORTS
#define GSM_DLL_SPEC __declspec(dllexport)
#define GSM_DLL_C_SPEC GSM_C_EXTERNS __declspec(dllexport)
#else
#define GSM_DLL_SPEC __declspec(dllimport)
#define GSM_DLL_C_SPEC GSM_C_EXTERNS __declspec(dllimport)
#endif
#else	// EZ_WINDOWS
#define GSM_DLL_SPEC
#define GSM_DLL_C_SPEC
#endif

#endif // __GAME_SVR_MGR_H__

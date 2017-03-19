/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/11/28
/************************************************************************/
#ifndef __CORE_SERVICE_H__
#define __CORE_SERVICE_H__

#include "xEzUtil.h"

#ifdef __cplusplus
	#define SVC_C_EXTERNS extern "C"
#else
	#define SVC_C_EXTERNS
#endif

#ifdef EZ_WINDOWS
	#ifdef CORE_SERVICE_EXPORTS
		#define SVC_DLL_SPEC __declspec(dllexport)
		#define SVC_DLL_C_SPEC SVC_C_EXTERNS __declspec(dllexport)
	#else
		#define SVC_DLL_SPEC __declspec(dllimport)
		#define SVC_DLL_C_SPEC SVC_C_EXTERNS __declspec(dllimport)
	#endif
#else	// EZ_WINDOWS
	#define SVC_DLL_SPEC
	#define SVC_DLL_C_SPEC
#endif

#ifdef EZ_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#endif


enum SVCErrorCode
{
	eOk = 0,							// no error
	eCreateSockFail,
	eBindSockFail,
	eListenFail,
	eSystemError,						// call system API fails
	eNotInitialized,
	eInvalidParameters,
	eNullPointer,
	eInvalidPackage,
	eSockClosed,
	eBufNotEnough,
	eNotImplementedYet,
};

#endif // __CORE_SERVICE_H__
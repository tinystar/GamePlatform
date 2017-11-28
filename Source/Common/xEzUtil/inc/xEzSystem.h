/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/11/28
/************************************************************************/
#ifndef __XEZ_SYSTEM_H__
#define __XEZ_SYSTEM_H__

#include "xEzCompile.h"
#include "xEzTypes.h"
#include "xEzMacro.h"
#include <tchar.h>

EZ_NAMESPACE_BEGIN(EzSys)

EZ_DLL_C_SPEC bool pathFileExists(const TCHAR* pszPath);
EZ_DLL_C_SPEC bool createDirectory(const TCHAR* pszPath);

EZ_DLL_C_SPEC EzUInt getProcessorCount();

EZ_DLL_C_SPEC bool getCurModulePath(TCHAR* pszPathBuffer, unsigned long nSizeInWords);


typedef void*	HEZMODULE;

EZ_DLL_C_SPEC HEZMODULE loadLibrary(const char* pszModName);
EZ_DLL_C_SPEC void freeLibrary(HEZMODULE hModule);
EZ_DLL_C_SPEC void* getSymbolAddress(HEZMODULE hModule, const char* pszSymbol);
EZ_DLL_C_SPEC HEZMODULE getModuleHandle(const char* pszModName);

EZ_NAMESPACE_END

#endif // __XEZ_SYSTEM_H__
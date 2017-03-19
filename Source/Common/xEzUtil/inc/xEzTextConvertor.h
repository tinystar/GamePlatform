/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/12/14
/************************************************************************/
#ifndef __XEZ_TEXT_CONVERTOR_H__
#define __XEZ_TEXT_CONVERTOR_H__

#include "xEzCompile.h"
#include "xEzMacro.h"
#include <tchar.h>

EZ_NAMESPACE_BEGIN(EzText)

// return values need to be deleted
EZ_DLL_C_SPEC char* wideCharToUtf8(const wchar_t* pSrc);
EZ_DLL_C_SPEC char* wideCharToAnsi(const wchar_t* pSrc);
EZ_DLL_C_SPEC wchar_t* ansiToWideChar(const char* pSrc);
EZ_DLL_C_SPEC char* ansiToUtf8(const char* pSrc);
/**
 * convert wchar_t to utf8 if _UNICODE defined
 * otherwise convert ansi to utf8
 */
EZ_DLL_C_SPEC char* tcharToUtf8(const TCHAR* pSrc);

EZ_NAMESPACE_END

#endif // __XEZ_TEXT_CONVERTOR_H__
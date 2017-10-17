#include "xEzTextConvertor.h"
#include "xEzDebug.h"
#include <windows.h>


EZ_NAMESPACE_BEGIN(EzText)

char* wideCharToUtf8(const wchar_t* pSrc)
{
	if (NULL == pSrc)
		return NULL;

	int len = ::WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, NULL, 0, NULL, NULL);
	char* pszResult = new char[len];
	if (!EzVerify(pszResult))
		return NULL;

	int cLen = ::WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, pszResult, len, NULL, NULL);
	if (!EzVerify(cLen > 0))
	{
		delete pszResult;
		return NULL;
	}

	return pszResult;
}

char* wideCharToAnsi(const wchar_t* pSrc)
{
	if (NULL == pSrc)
		return NULL;

	int len = ::WideCharToMultiByte(CP_ACP, 0, pSrc, -1, NULL, 0, NULL, NULL);
	char* pszResult = new char[len];
	if (!EzVerify(pszResult))
		return NULL;

	int cLen = ::WideCharToMultiByte(CP_ACP, 0, pSrc, -1, pszResult, len, NULL, NULL);
	if (!EzVerify(cLen > 0))
	{
		delete pszResult;
		return NULL;
	}

	return pszResult;
}

wchar_t* ansiToWideChar(const char* pSrc)
{
	if (NULL == pSrc)
		return NULL;

	int len = ::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[len];
	if (!EzVerify(pUnicode))
		return NULL;

	int cLen = ::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, pUnicode, len);
	if (!EzVerify(cLen > 0))
	{
		delete pUnicode;
		return NULL;
	}

	return pUnicode;
}

wchar_t* utf8ToWideChar(const char* pSrc)
{
	if (NULL == pSrc)
		return NULL;

	int len = ::MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[len];
	if (!EzVerify(pUnicode))
		return NULL;

	int cLen = ::MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, pUnicode, len);
	if (!EzVerify(cLen > 0))
	{
		delete pUnicode;
		return NULL;
	}

	return pUnicode;
}

char* ansiToUtf8(const char* pSrc)
{
	wchar_t* pUnicode = ansiToWideChar(pSrc);
	if (NULL == pUnicode)
		return NULL;

	char* pszResult = wideCharToUtf8(pUnicode);
	delete pUnicode;

	return pszResult;
}

char* tcharToUtf8(const TCHAR* pSrc)
{
#ifdef _UNICODE
	return wideCharToUtf8(pSrc);
#else
	return ansiToUtf8(pSrc);
#endif
}

EZ_NAMESPACE_END
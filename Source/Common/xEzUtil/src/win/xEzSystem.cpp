#include "xEzSystem.h"
#include "xEzDebug.h"
#include <windows.h>


EZ_NAMESPACE_BEGIN(EzSys)

bool pathFileExists(const TCHAR* pszPath)
{
	if (NULL == pszPath || 0 == *pszPath)
		return false;

	return (0 == _taccess(pszPath, 0));
}

bool createDirectory(const TCHAR* pszPath)
{
	if (NULL == pszPath || 0 == *pszPath)
		return false;

	size_t len = _tcslen(pszPath);
	if (len >= MAX_PATH)
		return false;

	TCHAR szDup[MAX_PATH + 1] = { 0 };
	_tcscpy_s(szDup, MAX_PATH, pszPath);
	_tcscat_s(szDup + len, MAX_PATH + 1 - len, _T("\\"));

	TCHAR* pCh = szDup;
	while (*pCh)
	{
		if (_T('\\') == *pCh || _T('/') == *pCh)
		{
			*pCh = _T('\0');
			if (_tcslen(szDup) > 0 && !pathFileExists(szDup))
			{
				BOOL bRet = ::CreateDirectory(szDup, NULL);
				if (!EzVerify(bRet))
					return false;
			}
			*pCh = _T('\\');
		}
		++pCh;
	}

	return true;
}

EzUInt getProcessorCount()
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);

	return sysInfo.dwNumberOfProcessors;
}

bool getCurModulePath(TCHAR* pszPathBuffer, unsigned long nSizeInWords)
{
	HMODULE hCurMod = ::GetModuleHandle(NULL);
	if (NULL == hCurMod)
		return false;

	TCHAR szFileName[MAX_PATH] = { 0 };
	DWORD dwRet = ::GetModuleFileName(hCurMod, szFileName, MAX_PATH);
	if (0 == dwRet)
		return false;

	DWORD i = 0;
	for (i = dwRet; i >= 0; --i)
	{
		if (szFileName[i] == _T('\\') && i < dwRet)
		{
			szFileName[i + 1] = 0;
			break;
		}
	}

	if (i >= nSizeInWords)
		return false;

	_tcscpy_s(pszPathBuffer, nSizeInWords, szFileName);
	return true;
}

HEZMODULE loadLibrary(const char* pszModName)
{
	return (HEZMODULE)::LoadLibraryA(pszModName);
}

void freeLibrary(HEZMODULE hModule)
{
	::FreeLibrary((HMODULE)hModule);
}

void* getSymbolAddress(HEZMODULE hModule, const char* pszSymbol)
{
	return (void*)::GetProcAddress((HMODULE)hModule, pszSymbol);
}

HEZMODULE getModuleHandle(const char* pszModName)
{
	return (HEZMODULE)::GetModuleHandleA(pszModName);
}

EZ_NAMESPACE_END

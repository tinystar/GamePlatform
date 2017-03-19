#include "xEzSystem.h"
#include "xEzDebug.h"
#include <windows.h>


bool EzSys::pathFileExists(const TCHAR* pszPath)
{
	if (NULL == pszPath || 0 == *pszPath)
		return false;

	return (0 == _taccess(pszPath, 0));
}

bool EzSys::createDirectory(const TCHAR* pszPath)
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

EzUInt EzSys::getProcessorCount()
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);

	return sysInfo.dwNumberOfProcessors;
}

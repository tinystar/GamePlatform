#include "xEzDebug.h"
#include <malloc.h>
#ifdef EZ_WINDOWS
#include <windows.h>
#elif defined(EZ_LINUX)
// todo
#endif

void MyTrace(const TCHAR* fmt, ...)
{
	if (NULL == fmt)
		return;

	va_list args;
	va_start(args, fmt);

	int len = _vsctprintf(fmt, args);
	TCHAR* pszBuf = (TCHAR*)_alloca((len + 1)*sizeof(TCHAR));	// alloc on stack, need not be free.
	if (!EzVerify(pszBuf))
		return;

	_vsntprintf_s(pszBuf, len + 1, len, fmt, args);
	va_end(args);

#ifdef EZ_WINDOWS
	OutputDebugString(pszBuf);
#else
	// todo
#endif
}
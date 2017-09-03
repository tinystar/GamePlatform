#include "xEzLog.h"
#include "xEzDebug.h"
#ifdef EZ_WINDOWS
#include "win/xEzLogImp.h"
#elif defined(EZ_LINUX)
// todo
#endif

static EzLoggerImp* _s_pImpLogger = NULL;


bool EzLogger::init(const TCHAR* pszLogName, LogLevel level /*= kLogAll*/)
{
	if (NULL == _s_pImpLogger)
	{
		_s_pImpLogger = new EzLoggerImp(pszLogName, level);
		EzAssert(_s_pImpLogger != NULL);
		return _s_pImpLogger->start();
	}
	return true;
}

void EzLogger::exit()
{
	if (_s_pImpLogger != NULL)
	{
		_s_pImpLogger->stop();
		delete _s_pImpLogger;
		_s_pImpLogger = NULL;
	}
}

bool EzLogger::log(LogLevel level, const TCHAR* pszFormat, ...)
{
	if (NULL == _s_pImpLogger)
		return false;

	va_list vargs;
	va_start(vargs, pszFormat);

	return _s_pImpLogger->_log(level, pszFormat, vargs);
}

bool EzLogger::flush()
{
	if (NULL == _s_pImpLogger)
		return false;

	return _s_pImpLogger->_flush();
}

void EzLogger::setLogLevel(LogLevel level)
{
	if (NULL == _s_pImpLogger)
		return;

	_s_pImpLogger->setLogLevel(level);
}

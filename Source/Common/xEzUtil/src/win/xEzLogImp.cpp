#include "xEzLogImp.h"
#include "xEzMacro.h"
#include "xEzDebug.h"
#include <process.h>
#include "xEzTime.h"
#include "xEzTextConvertor.h"
#include "xEzSystem.h"

using namespace std;
using namespace EzText;
using namespace EzSys;
using namespace EzTime;

#define DEF_LOG_NAME	_T("default")


EzLoggerImp::EzLoggerImp(const TCHAR* pszLogName, LogLevel level)
	: m_logLevel(level)
	, m_pCurBuffer(NULL)
	, m_hWriteEvent(NULL)
	, m_hQuitEvent(NULL)
	, m_bRunning(false)
	, m_hLogThread(INVALID_HANDLE_VALUE)
	, m_startTime(0)
{
	m_pCurBuffer = new LogBuffer();
	EzAssert(m_pCurBuffer != NULL);

	// create two backup buffers by default.
	for (int i = 0; i < 2; ++i)
		m_freeList.push_back(new LogBuffer());

	::memset(m_szLogName, 0, sizeof(m_szLogName));
	::memset(m_szFilePath, 0, sizeof(m_szFilePath));

	if (NULL == pszLogName || 0 == *pszLogName)
		_tcsncpy_s(m_szLogName, DEF_LOG_NAME, 255);
	else
		_tcsncpy_s(m_szLogName, pszLogName, 255);

	generateFilePath();
}

EzLoggerImp::~EzLoggerImp()
{
	cleanBuffers();
}

bool EzLoggerImp::_log(LogLevel level, const TCHAR* pszFormat, va_list vargs)
{
	if (!m_bRunning)
		return false;

	if (level > m_logLevel)
		return false;
	if (kLogOff == level || kLogAll == level)
		return false;
	if (NULL == pszFormat)
		return false;

	const TCHAR* pszLevel = levelToString(level);

	int len1 = _tcslen(pszLevel);
	int len2 = _vsctprintf(pszFormat, vargs);
	int len = len1 + len2 + 2;
	TCHAR* pszLogBuf = (TCHAR*)_alloca((len + 1)*sizeof(TCHAR));	// alloc on stack, need not be free.
	if (!EzVerify(pszLogBuf))
		return false;
	
	int nLen = _sntprintf_s(pszLogBuf, len + 1, len, _T("[%s]"), pszLevel);
	if (nLen < 0)
		return false;

	nLen = _vsntprintf_s(pszLogBuf + nLen, len + 1 - nLen, len2, pszFormat, vargs);
	if (nLen < 0)
		return false;
	*(pszLogBuf + len) = 0;	// end string

	return appendLog(pszLogBuf);
}

bool EzLoggerImp::start()
{
	if (m_bRunning)
		return true;

	m_hWriteEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!EzVerify(m_hWriteEvent != NULL))
		return false;

	m_hQuitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);	// manual reset
	if (!EzVerify(m_hQuitEvent != NULL))
		return false;

	m_LogFile.open(m_szFilePath, ios::app);
	if (!EzVerify(m_LogFile))
	{
		EzTrace(_T("create log file failure!"));
		return false;
	}

	m_startTime = time(NULL);

	m_hLogThread = (HANDLE)_beginthreadex(NULL, 0, writeLogThread, this, CREATE_SUSPENDED, NULL);
	if (!EzVerify(m_hLogThread != INVALID_HANDLE_VALUE))
	{
		EzTrace(_T("create log thread failure! error code: %d"), GetLastError());
		return false;
	}
	//::SetThreadPriority(m_hLogThread, THREAD_PRIORITY_BELOW_NORMAL);		// 调低写log线程优先级
	::ResumeThread(m_hLogThread);

	m_bRunning = true;

	return true;
}

bool EzLoggerImp::stop()
{
	if (!m_bRunning)
		return true;

	_flush();
	m_bRunning = false;			// after flush set running flag to false to prevent other thread continue to log.
	::SetEvent(m_hQuitEvent);	// to terminate the write thread

	// wait the write thread to exit.
	DWORD dwErr = ::WaitForSingleObject(m_hLogThread, INFINITE);
	EzAssert(WAIT_OBJECT_0 == dwErr);
	::CloseHandle(m_hLogThread);
	m_hLogThread = INVALID_HANDLE_VALUE;

	m_startTime = 0;
	m_LogFile.close();

	::CloseHandle(m_hQuitEvent);
	m_hQuitEvent = NULL;

	::CloseHandle(m_hWriteEvent);
	m_hWriteEvent = NULL;

	return true;
}

void EzLoggerImp::cleanBuffers()
{
	EZ_SAFE_DELETE(m_pCurBuffer);

	BufListQueue::const_iterator citer = m_fullList.cbegin();
	for (; citer != m_fullList.cend(); ++citer)
	{
		delete *citer;
	}
	m_fullList.clear();

	citer = m_freeList.cbegin();
	for (; citer != m_freeList.cend(); ++citer)
	{
		delete *citer;
	}
	m_freeList.clear();
}

bool EzLoggerImp::generateFilePath()
{
	if (_tcslen(m_szLogName) <= 0)
		return false;

	m_szFilePath[0] = 0;

	TCHAR szDrive[256] = { 0 };
	TCHAR szDir[256] = { 0 };
	TCHAR szFile[256] = { 0 };
	TCHAR szExt[256] = { 0 };
	_tsplitpath_s(m_szLogName, szDrive, szDir, szFile, szExt);

	if (0 == *szExt)
		_tcsncpy_s(szExt, _T(".log"), 255);

	TCHAR szPath[256] = { 0 };
	_sntprintf_s(szPath, 256, 255, _T("%s%s"), szDrive, szDir);
	if (_tcslen(szPath) > 0 && !pathFileExists(szPath))
	{
		bool bRet = createDirectory(szPath);
		EzAssert(bRet);
	}

	int year, month, day;
	year = month = day = 0;
	getDateToday(year, month, day);

	_sntprintf_s(m_szFilePath, 256, 255, _T("%s%s%s_%d_%d_%d%s"), szDrive, szDir, szFile, year, month, day, szExt);

	return true;
}

const TCHAR* EzLoggerImp::levelToString(LogLevel level) const
{
	const TCHAR* pszLevel = _T("");
	switch (level)
	{
	case kLogFatal:
		pszLevel = _T("FATAL");
		break;
	case kLogError:
		pszLevel = _T("ERROR");
		break;
	case kLogWarn:
		pszLevel = _T("WARN");
		break;
	case kLogInfo:
		pszLevel = _T("INFO");
		break;
	case kLogDebug:
		pszLevel = _T("DEBUG");
		break;
	default:
		pszLevel = _T("UNKNOWN");
		break;
	}
	return pszLevel;
}

bool EzLoggerImp::appendLog(TCHAR* pszLog)
{
	if (NULL == pszLog)
		return false;

	EzAutoLocker locker(&m_listLock);

	if (!EzVerify(m_pCurBuffer != NULL))
		return false;

	const char* pszUtf8 = tcharToUtf8(pszLog);
	int nRet = m_pCurBuffer->write(pszUtf8, strlen(pszUtf8));
	if (-1 == nRet)		// full
	{
		bool bRet = _flush();	// flush the buffer when it's full.
		EzAssert(bRet && m_pCurBuffer);
		// after flush, write to new buffer again.
		nRet = m_pCurBuffer->write(pszUtf8, strlen(pszUtf8));
		EzAssert(nRet > 0);
	}
	::free((void*)pszUtf8);

	return true;
}

unsigned EZSTDCALL EzLoggerImp::writeLogThread(void* pParam)
{
	EzLoggerImp* pLogger = (EzLoggerImp*)pParam;
	if (!EzVerify(pLogger))
		return -1;
	HANDLE hEvents[2] = { pLogger->m_hWriteEvent, pLogger->m_hQuitEvent };

	while (true)
	{
		try
		{
			DWORD dwRet = ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
			if (WAIT_OBJECT_0 == dwRet)
				pLogger->writeLog();
			else if ((WAIT_OBJECT_0 + 1) == dwRet)
				break;	// quit
			else
				EzTrace(_T("wait error!"));
		}
		catch (...)
		{
			EzTrace(_T("exception occurred in write log thread!"));
			pLogger->writeLog();
			return -1;
		}
	}

	return 0;
}

bool EzLoggerImp::writeLog()
{
	while (m_fullList.size() > 0)		// 单消费者线程，可以这样写，如果是多消费者线程则判断size的操作也需要同步
	{
		LogBuffer* pFrontBuf = NULL;
		{// need to be protected code block
			EzAutoLocker locker(&m_listLock);

			pFrontBuf = m_fullList.front();
			m_fullList.pop_front();
		}
		if (!EzVerify(pFrontBuf))
		{
			EzTrace(_T("Null buffer!!!"));
			continue;
		}

		bool bRet = pFrontBuf->flush(m_LogFile);
		m_LogFile.flush();
		if (!EzVerify(bRet))
			EzTrace(_T("write to log file error!"));

		{// need to be protected code block
			EzAutoLocker locker(&m_listLock);
			m_freeList.push_back(pFrontBuf);
		}
	}

	if (checkNextDay())
	{
		m_startTime = time(NULL);

		generateFilePath();		// 新的一天重新生成文件名

		m_LogFile.close();
		m_LogFile.clear();
		m_LogFile.open(m_szFilePath, ios::app);
		if (!EzVerify(m_LogFile))
			EzTrace(_T("create log file failure!"));
	}

	return true;
}

bool EzLoggerImp::_flush()
{
	if (!m_bRunning)
		return false;

	if (m_pCurBuffer == NULL)
		return false;

	if (m_pCurBuffer->isEmpty())
		return true;

	EzAutoLocker locker(&m_listLock);
	EzAssert(m_pCurBuffer);

	m_fullList.push_back(m_pCurBuffer);
	m_pCurBuffer = NULL;
	if (m_freeList.empty())
	{
		m_pCurBuffer = new LogBuffer();
	}
	else
	{
		m_pCurBuffer = m_freeList.front();
		m_freeList.pop_front();
	}
	EzAssert(m_pCurBuffer != NULL);

	BOOL bRet = ::SetEvent(m_hWriteEvent);	// notify the write thread to work
	if (!EzVerify(bRet))
		EzTrace(_T("set event failure! error code = %d\n"), GetLastError());

	return true;
}

bool EzLoggerImp::checkNextDay()
{
	int year1, month1, day1;
	year1 = month1 = day1 = 0;
	getDateToday(year1, month1, day1);

	int year2, month2, day2;
	year2 = month2 = day2 = 0;
	getDateFromTime(m_startTime, year2, month2, day2);

	return (day1 != day2) || (month1 != month2) || (year1 != year2);
}

void EzLoggerImp::setLogLevel(LogLevel level)
{
	m_logLevel = level;
}

/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/12/3
/*		 2017/1/19 cross-platform modified.
/************************************************************************/
#ifndef __XEZ_LOG_IMP_H__
#define __XEZ_LOG_IMP_H__

#include "xEzLog.h"
#include "xEzLock.h"
#include "xEzBuffer.h"
#include <list>
#include <fstream>
#include <windows.h>
#include <time.h>

#define LOG_BUF_SIZE	(1*1024)


class EzLoggerImp
{
	friend class EzLogger;

private:
	EzLoggerImp(const TCHAR* pszLogName, LogLevel level);
	~EzLoggerImp();
	// none copyable
	EzLoggerImp(const EzLoggerImp&);
	EzLoggerImp& operator= (const EzLoggerImp&);

protected:
	bool start();
	bool stop();

	void setLogLevel(LogLevel level);

	bool generateFilePath();
	const TCHAR* levelToString(LogLevel level) const;
	void cleanBuffers();

	bool _log(LogLevel level, const TCHAR* pszFormat, va_list vargs);
	bool appendLog(TCHAR* pszLog);

	bool _flush();

	bool writeLog();

	bool checkNextDay();

	static unsigned EZSTDCALL writeLogThread(void* pParam);

private:
	typedef EzFixedBuf<char, LOG_BUF_SIZE>	LogBuffer;
	typedef std::list<LogBuffer*>		BufListQueue;

	LogLevel			m_logLevel;
	LogBuffer*			m_pCurBuffer;
	BufListQueue		m_fullList;
	BufListQueue		m_freeList;
	EzLock				m_listLock;
	HANDLE				m_hWriteEvent;
	HANDLE				m_hQuitEvent;
	HANDLE				m_hLogThread;
	bool				m_bRunning;
	std::ofstream		m_LogFile;
	time_t				m_startTime;
	TCHAR				m_szLogName[256];
	TCHAR				m_szFilePath[256];
};

#endif // __XEZ_LOG_IMP_H__
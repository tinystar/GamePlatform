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
#ifndef __XEZ_LOG_H__
#define __XEZ_LOG_H__

#include <tchar.h>

enum LogLevel
{
	kLogOff,
	kLogFatal,
	kLogError,
	kLogWarn,
	kLogInfo,
	kLogDebug,
	kLogAll
};

// thread safe and async log class
class EzLogger
{
public:
	static bool init(const TCHAR* pszLogName, LogLevel level = kLogAll);
	// please call exit() after init() has called, otherwise it will cause memory leaks.
	static void exit();

	static bool log(LogLevel level, const TCHAR* pszFormat, ...);
	static bool flush();

private:
	EzLogger();
	~EzLogger();
	// none copyable
	EzLogger(const EzLogger&);
	EzLogger& operator= (const EzLogger&);
};


#define EzLogInit1(name)			EzLogger::init(name)
#define EzLogInit2(name, level)		EzLogger::init(name, level)
#define EzLogExit					EzLogger::exit
#define EzLogFlush					EzLogger::flush

#define EzLog(level, fmt, ...)		EzLogger::log(level, fmt, ##__VA_ARGS__)
#define EzLogFatal(fmt, ...)		EzLogger::log(kLogFatal, fmt, ##__VA_ARGS__)
#define EzLogError(fmt, ...)		EzLogger::log(kLogError, fmt, ##__VA_ARGS__)
#define EzLogWarn(fmt, ...)			EzLogger::log(kLogWarn, fmt, ##__VA_ARGS__)
#define EzLogInfo(fmt, ...)			EzLogger::log(kLogInfo, fmt, ##__VA_ARGS__)
#define EzLogDebug(fmt, ...)		EzLogger::log(kLogDebug, fmt, ##__VA_ARGS__)

#endif // __XEZ_LOG_H__
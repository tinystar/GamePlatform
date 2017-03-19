/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/18
/************************************************************************/
#ifndef __XEZ_PLATFORM_H__
#define __XEZ_PLATFORM_H__

#define EZ_PLATFORM_UNKNOWN				0
#define EZ_PLATFORM_WINDOWS				1
#define EZ_PLATFORM_LINUX				2


#if defined(_WIN32) && defined(_WINDOWS)
	#define EZ_TARGET_PLATFORM			EZ_PLATFORM_WINDOWS
#elif defined(LINUX)
	#define EZ_TARGET_PLATFORM			EZ_PLATFORM_LINUX
#else
	#define EZ_TARGET_PLATFORM			EZ_PLATFORM_UNKNOWN
#endif

#if (EZ_TARGET_PLATFORM == EZ_PLATFORM_WINDOWS)
	#define EZ_WINDOWS
#elif (EZ_TARGET_PLATFORM == EZ_PLATFORM_LINUX)
	#define EZ_LINUX
#else
	#error "unknown plateform!"
#endif

#endif // __XEZ_PLATFORM_H__
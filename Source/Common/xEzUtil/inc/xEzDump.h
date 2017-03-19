/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/2/14
/************************************************************************/
#ifndef __XEZ_DUMP_H__
#define __XEZ_DUMP_H__

#include "xEzCompile.h"
#include "xEzMacro.h"
#include "xEzTypes.h"
#include <iosfwd>

class EZ_DLL_SPEC EzDumpContext
{
EZ_FORBID_CLASS_COPY(EzDumpContext)

public:
	explicit EzDumpContext(std::ostream& os)
		: m_ostream(os)
	{}

	EzDumpContext& operator<< (const char* pszInfo);
	EzDumpContext& operator<< (const void* pPointer);
	EzDumpContext& operator<< (char c);
	EzDumpContext& operator<< (unsigned char uc);
	EzDumpContext& operator<< (int i);
	EzDumpContext& operator<< (unsigned int ui);
	EzDumpContext& operator<< (short s);
	EzDumpContext& operator<< (unsigned short us);
	EzDumpContext& operator<< (long l);
	EzDumpContext& operator<< (unsigned long ul);
	EzDumpContext& operator<< (EzInt64 i64);
	EzDumpContext& operator<< (EzUInt64 ui64);
	EzDumpContext& operator<< (float f);
	EzDumpContext& operator<< (double d);

	void flush();

protected:
	std::ostream& m_ostream;
};

#endif // __XEZ_DUMP_H__
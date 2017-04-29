/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/4/14
/************************************************************************/
#ifndef __XEZ_MEMORY_H__
#define __XEZ_MEMORY_H__

#include "xEzCompile.h"
#include "xEzMacro.h"

EZ_DLL_C_SPEC void* EzMalloc(size_t size);
EZ_DLL_C_SPEC void EzFree(void* pPointer);

class EzHeapOper
{
private:
	EZ_DLL_SPEC static void* allocRawMem(size_t size);
	EZ_DLL_SPEC static void* allocRawMem(size_t size, const char* pFile, int nLine);

	EZ_DLL_SPEC static void freeRawMem(void* p);
	EZ_DLL_SPEC static void freeRawMem(void* p, const char* pFile, int nLine);

public:
#undef new
#undef delete

	static void* operator new(size_t size)
	{
		void* p = allocRawMem(size);
		return p;
	}

	static void* operator new[](size_t size)
	{
		void* p = allocRawMem(size);
		return p;
	}

	static void* operator new(size_t size, const char* pFile, int nLine)
	{
		void* p = allocRawMem(size, pFile, nLine);
		return p;
	}

	static void* operator new[](size_t size, const char* pFile, int nLine)
	{
		void* p = allocRawMem(size, pFile, nLine);
		return p;
	}

	static void operator delete(void* p)
	{
		if (p != NULL)
			freeRawMem(p);
	}

	static void operator delete[](void* p)
	{
		if (p != NULL)
			freeRawMem(p);
	}

	static void operator delete(void* p, const char* pFile, int nLine)
	{
		if (p != NULL)
			freeRawMem(p, pFile, nLine);
	}

	static void operator delete[](void* p, const char* pFile, int nLine)
	{
		if (p != NULL)
			freeRawMem(p, pFile, nLine);
	}
};

#endif // __XEZ_MEMORY_H__
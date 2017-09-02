#include "xEzMemory.h"
#include "xEzMemoryPool.h"


//------------------------------------------------------
// EzMalloc/EzFree
//------------------------------------------------------
#ifdef XEZ_USE_MEMORY_POOL
extern EzMemPool<EzMemPoolConfig> g_memPool;
#endif

void* EzMalloc(size_t size)
{
#ifdef XEZ_USE_MEMORY_POOL
	return g_memPool.allocMem(size);
#else
	return ::malloc(size);
#endif
}

void EzFree(void* pPointer)
{
#ifdef XEZ_USE_MEMORY_POOL
	g_memPool.freeMem(pPointer);
#else
	::free(pPointer);
#endif
}


//------------------------------------------------------
// EzHeapOper
//------------------------------------------------------
void* EzHeapOper::allocRawMem(size_t size)
{
	return EzMalloc(size);
}

void* EzHeapOper::allocRawMem(size_t size, const char* pFile, int nLine)
{
	return ::_malloc_dbg(size, 1, pFile, nLine);
}

void EzHeapOper::freeRawMem(void* p)
{
	EzFree(p);
}

void EzHeapOper::freeRawMem(void* p, const char* pFile, int nLine)
{
	::_free_dbg(p, 1);
}

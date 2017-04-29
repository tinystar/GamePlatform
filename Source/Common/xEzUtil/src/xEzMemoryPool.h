/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/4/14
/************************************************************************/
#ifndef __XEZ_MEMORY_POOL_H__
#define __XEZ_MEMORY_POOL_H__

#include "xEzTypes.h"
#include "xEzDebug.h"
#include "xEzMacro.h"
#include <map>
#include "xEzLock.h"
#include "xEzException.h"

class EzMemPoolConfig
{
public:
	enum PoolParamDef
	{
		kChunkSize	= 64 * 1024,		// 512k
		kChunkCount	= 4 * 1024,			// max memory pool size is 2G
		kBlockCount = 60
	};

	static int sGetBlockIndex(size_t size);
	static size_t sGetBlockSize(int index);

private:
	static const size_t	s_blockSizes[kBlockCount + 1];
	static const EzByte s_memSizeToBlockIndex[257];
};

// used for std map's key.
class EzChunkMemRange
{
public:
	EzChunkMemRange(const void* pAddrStart, const void* pAddrEnd)
		: m_pAddressMin(0)
		, m_pAddressMax(0)
	{
		EzAssert(pAddrStart <= pAddrEnd);
		m_pAddressMin = EZMIN(pAddrStart, pAddrEnd);
		m_pAddressMax = EZMAX(pAddrStart, pAddrEnd);
	}

	EzChunkMemRange(const void* pAddress)
		: m_pAddressMin(pAddress)
		, m_pAddressMax(pAddress)
	{
	}

	bool operator< (const EzChunkMemRange& rhs) const
	{
		return m_pAddressMax < rhs.m_pAddressMin;
	}

private:
	const void* m_pAddressMin;
	const void* m_pAddressMax;
};

template<typename TPoolConfig>
class EzMemPool
{
public:
	EzMemPool();
	~EzMemPool();

	void* allocMem(size_t size);

	void freeMem(void* pPointer);

private:
	struct EzBlock
	{
		EzBlock* next;
	};

	struct EzChunk
	{
		size_t blockSize;
		EzBlock* blocks;
	};

	enum
	{
		kANodeChunkCount = 256
	};

	// 在池中内存分配很少的情况下，为了减少池本身的内存占用，将Chunk的记录信息分成一段一段的，
	// 用链表进行组织以免一次性分配TPoolConfig::kChunkCount大小的EzChunk数组
	struct EzChunksNode
	{
		EzChunk chunks[kANodeChunkCount];
		EzChunksNode* next;
	};

private:
	typedef std::map<EzChunkMemRange, int>	MemAddressToBlockIdxMap;

	EzChunksNode*				m_pChunksHead;
	EzChunksNode*				m_pCurChunks;
	EzBlock*					m_freeBlocks[TPoolConfig::kBlockCount + 1];
	MemAddressToBlockIdxMap		m_mapAddrToBlockIdx;
	int							m_nUsedChunkCount;
	int							m_nMaxChunkCount;
	EzLock						m_lock;
};

template<typename TPoolConfig>
EzMemPool<TPoolConfig>::EzMemPool()
	: m_pChunksHead(NULL)
	, m_pCurChunks(NULL)
	, m_nUsedChunkCount(0) 
	, m_nMaxChunkCount(0)
{
	m_pChunksHead = new EzChunksNode();
	if (EzVerify(m_pChunksHead != NULL))
	{
		m_pCurChunks = m_pChunksHead;
		::memset(m_pChunksHead, 0, sizeof(EzChunksNode));
		m_nMaxChunkCount = kANodeChunkCount;
	}

	for (int i = 0; i <= TPoolConfig::kBlockCount; ++i)
	{
		m_freeBlocks[i] = NULL;
	}
}

template<typename TPoolConfig>
EzMemPool<TPoolConfig>::~EzMemPool()
{
	EzChunksNode* pNode = m_pChunksHead;
	while (pNode)
	{
		for (int i = 0; i < kANodeChunkCount; ++i)
		{
			if (pNode->chunks[i].blocks != NULL)
				::free(pNode->chunks[i].blocks);
		}

		EzChunksNode* pCur = pNode;
		pNode = pNode->next;
		delete pCur;
	}
}

template<typename TPoolConfig>
void* EzMemPool<TPoolConfig>::allocMem(size_t size)
{
	if (0 == size)
		return NULL;

	int idx = TPoolConfig::sGetBlockIndex(size);
	if (!EzVerify(0 <= idx && idx <= TPoolConfig::kBlockCount))
		return NULL;

	if (0 == idx)
		return ::malloc(size);

	EzAutoLocker _autoLocker(&m_lock);

	EzBlock* pBlock = m_freeBlocks[idx];
	if (pBlock)
	{
		m_freeBlocks[idx] = pBlock->next;
		return pBlock;
	}
	else
	{
		if (m_nUsedChunkCount >= TPoolConfig::kChunkCount)
		{
			// reached the limit of the pool. allocate memory from system.
			return ::malloc(size);
		}

		if (m_nUsedChunkCount >= m_nMaxChunkCount)
		{
			EzChunksNode* pNewNode = new EzChunksNode();
			if (NULL == pNewNode)
				throw EzException(_T("allocate EzChunksNode memory fails!"), _T(__FILE__), __LINE__);

			::memset(pNewNode, 0, sizeof(EzChunksNode));
			m_pCurChunks->next = pNewNode;
			m_pCurChunks = pNewNode;
			m_nMaxChunkCount += kANodeChunkCount;
		}

		void* pAChunk = ::malloc(TPoolConfig::kChunkSize);
		if (NULL == pAChunk)
			return NULL;

		int idxCurChunk = m_nUsedChunkCount % kANodeChunkCount;
		EzChunk* pChunk = &m_pCurChunks->chunks[idxCurChunk];
		EzAssert(pChunk != NULL);

		EzChunkMemRange key(pAChunk, (EzByte*)pAChunk + TPoolConfig::kChunkSize);
		m_mapAddrToBlockIdx.insert(std::make_pair(key, idx));

		size_t blockSize = TPoolConfig::sGetBlockSize(idx);
		EzBlock* pStartBlock = (EzBlock*)pAChunk;
		pChunk->blockSize = blockSize;
		pChunk->blocks = pStartBlock;

		size_t blockCount = TPoolConfig::kChunkSize / blockSize;
		for (size_t i = 0; i < blockCount - 1; ++i)
		{
			EzBlock* pCurrent = (EzBlock*)((EzByte*)pAChunk + blockSize * i);
			EzBlock* pNext = (EzBlock*)((EzByte*)pAChunk + blockSize *(i + 1));
			pCurrent->next = pNext;
		}
		EzBlock* pLast = (EzBlock*)((EzByte*)pAChunk + blockSize * (blockCount - 1));
		pLast->next = NULL;

		m_freeBlocks[idx] = pChunk->blocks->next;
		++m_nUsedChunkCount;

		return pStartBlock;
	}

	return NULL;
}

template<typename TPoolConfig>
void EzMemPool<TPoolConfig>::freeMem(void* pPointer)
{
	if (NULL == pPointer)
		return;

	EzAutoLocker _autoLocker(&m_lock);

	MemAddressToBlockIdxMap::iterator iter = m_mapAddrToBlockIdx.find(EzChunkMemRange(pPointer));
	if (iter == m_mapAddrToBlockIdx.end())
	{
		::free(pPointer);
		return;
	}

	int idx = iter->second;
	if (!EzVerify(0 < idx && idx <= TPoolConfig::kBlockCount))
	{
		EzTrace(_T("Error! should not be reached!"));
		return;
	}

	EzBlock* pBlock = (EzBlock*)pPointer;
	pBlock->next = m_freeBlocks[idx];
	m_freeBlocks[idx] = pBlock;
}

#endif // __XEZ_MEMORY_POOL_H__
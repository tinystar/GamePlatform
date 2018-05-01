/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/4/22
/************************************************************************/
#ifndef __XEZ_HEAP_ALG_H__
#define __XEZ_HEAP_ALG_H__

#include "xEzArray.h"

template<class T>
struct DefLess
{
	bool operator() (const T& _Left, const T& _Right) const
	{
		return (_Left < _Right);
	}
};

#define INIT_HEAP_SIZE		128

template<class T, class _Less = DefLess<T> >
class EzMiniHeap
{
public:
	explicit EzMiniHeap(int nInitSize = INIT_HEAP_SIZE)
		: m_heap(nInitSize)
	{}

	EzMiniHeap(T arr[], int n);
	~EzMiniHeap() { clear(); }

public:
	bool isEmpty() const { return m_heap.isEmpty(); }
	int size() const { return m_heap.length(); }

	void clear() { m_heap.removeAll(); }

	const EzArray<T>& toArray() const { return m_heap; }

	void insert(const T& val);
	bool removeMin(T& val);

protected:
	void shiftUp(int nStart);
	void shiftDown(int nStart);

protected:
	EzArray<T>		m_heap;
	_Less			m_lessCmp;
};


template<class T, class _Less>
EzMiniHeap<T, _Less>::EzMiniHeap(T arr[], int n)
	: m_heap(n)
{
	for (int i = 0; i < n; ++i)
		m_heap.append(arr[i]);

	int nLastIndex = m_heap.logicalLength() - 1;
	int nCurPos = (nLastIndex - 1) / 2;
	while (nCurPos >= 0)
	{
		shiftDown(nCurPos);
		--nCurPos;
	}
}

template<class T, class _Less>
void EzMiniHeap<T, _Less>::insert(const T& val)
{
	m_heap.append(val);
	shiftUp(m_heap.logicalLength() - 1);
}

template<class T, class _Less>
bool EzMiniHeap<T, _Less>::removeMin(T& val)
{
	if (isEmpty())
	{
		EzAssert(!_T("Heap is empty!"));
		return false;
	}

	val = m_heap[0];
	m_heap.swap(0, m_heap.logicalLength() - 1);
	m_heap.removeLast();
	shiftDown(0);
	return true;
}

template<class T, class _Less>
void EzMiniHeap<T, _Less>::shiftUp(int nStart)
{
	if (nStart <= 0)
		return;

	int nParent = (nStart - 1) / 2;
	T upNode = m_heap[nStart];
	while (nStart > 0)
	{
		if (!m_lessCmp(upNode, m_heap[nParent]))
		{
			break;
		}
		else
		{
			m_heap[nStart] = m_heap[nParent];
			nStart = nParent;
			nParent = (nStart - 1) / 2;
		}
	}
	m_heap[nStart] = upNode;
}

template<class T, class _Less>
void EzMiniHeap<T, _Less>::shiftDown(int nStart)
{
	if (nStart >= m_heap.logicalLength())
		return;

	int nLastIdx = m_heap.logicalLength() - 1;
	int nMinChild = 2 * nStart + 1;		// left child
	T downNode = m_heap[nStart];
	while (nMinChild <= nLastIdx)
	{
		// right child is lesser
		if (nMinChild < nLastIdx && m_lessCmp(m_heap[nMinChild + 1], m_heap[nMinChild]))
			++nMinChild;		// right child

		if (!m_lessCmp(m_heap[nMinChild], downNode))
		{
			break;
		}
		else
		{
			m_heap[nStart] = m_heap[nMinChild];
			nStart = nMinChild;
			nMinChild = 2 * nStart + 1;		// left child
		}
	}
	m_heap[nStart] = downNode;
}

#endif // __XEZ_HEAP_ALG_H__
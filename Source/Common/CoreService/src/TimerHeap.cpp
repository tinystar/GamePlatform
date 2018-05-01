#include "TimerHeap.h"


TimerHeap::TimerHeap()
{

}

TimerHeap::~TimerHeap()
{

}

bool TimerHeap::addTimer(EzUInt uTimerId, EzUInt uElapse, EzInt64 nExpires)
{
	for (int i = 0; i < m_heap.logicalLength(); ++i)
	{
		if (m_heap[i].timerId == uTimerId)
		{
			// update exist timer node
			m_heap[i].uElapse = uElapse;
			m_heap[i].expires = nExpires;
			adjustNode(i);
			return true;
		}
	}

	// add a new one
	insert(HeapTimerNode(uTimerId, uElapse, nExpires));
	return true;
}

bool TimerHeap::removeTimer(EzUInt uTimerId)
{
	for (int i = 0; i < m_heap.logicalLength(); ++i)
	{
		if (m_heap[i].timerId == uTimerId)
		{
			int nLastIdx = m_heap.logicalLength() - 1;
			if (i != nLastIdx)		// not last node
			{
				m_heap.swap(i, nLastIdx);
				m_heap.removeLast();
				adjustNode(i);
			}
			else
			{
				m_heap.removeLast();
			}

			return true;
		}
	}

	return false;
}

void TimerHeap::adjustNode(int idx)
{
	if (idx < 0)
		return;

	int nParentIdx = (idx - 1) / 2;
	if (idx > 0 && m_lessCmp(m_heap[idx], m_heap[nParentIdx]))
		shiftUp(idx);
	else
		shiftDown(idx);
}

bool TimerHeap::poll(HeapTimerNode& node)
{
	return removeMin(node);
}

const HeapTimerNode* TimerHeap::peek() const
{
	if (isEmpty())
		return NULL;

	return &m_heap[0];
}

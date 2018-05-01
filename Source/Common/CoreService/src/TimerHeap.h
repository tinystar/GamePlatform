/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/4/25
/************************************************************************/
#ifndef __TIMER_HEAP_H__
#define __TIMER_HEAP_H__

#include "xEzUtil.h"

struct HeapTimerNode
{
	EzUInt	timerId;
	EzUInt	uElapse;			// milliseconds
	EzInt64	expires;			// microseconds

	HeapTimerNode()
		: timerId(0)
		, uElapse(0)
		, expires(0)
	{}

	HeapTimerNode(EzUInt id, EzUInt elapse, EzInt64 exps)
		: timerId(id)
		, uElapse(elapse)
		, expires(exps)
	{}
};

struct TimerNodeLessCmp
{
	bool operator() (const HeapTimerNode& node1, const HeapTimerNode& node2) const
	{
		return node1.expires < node2.expires;
	}
};

// Using private inheritance to hide base class members
class TimerHeap : private EzMiniHeap<HeapTimerNode, TimerNodeLessCmp>
{
	typedef EzMiniHeap<HeapTimerNode, TimerNodeLessCmp> BaseClass;

public:
	TimerHeap();
	~TimerHeap();

public:
	bool addTimer(EzUInt uTimerId, EzUInt uElapse, EzInt64 nExpires);
	bool removeTimer(EzUInt uTimerId);

	bool poll(HeapTimerNode& node);
	const HeapTimerNode* peek() const;

	void clear() { BaseClass::clear(); }

protected:
	void adjustNode(int idx);
};

#endif // __TIMER_HEAP_H__
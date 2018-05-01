#include "TimerTask.h"
#include "xEzUtil.h"


//------------------------------------------------------
// TaskNode
//------------------------------------------------------
TaskNode::TaskNode()
	: m_pPrev(NULL)
	, m_pNext(NULL)
	, m_uRound(0)
{

}

TaskNode::~TaskNode()
{

}


//------------------------------------------------------
// TimerTask
//------------------------------------------------------
TimerTask::~TimerTask()
{

}

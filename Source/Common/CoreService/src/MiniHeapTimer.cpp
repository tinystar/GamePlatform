#include "MiniHeapTimer.h"
#include <process.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// ����һ����С��ֵ��֤�����˯�ߵ��趨�ļ��ʱ��
// ԭ�����趨һ���뾫�ȵ�����£��۲�Windows��ʵ��Sleep˯��ʱ�䷢��һ�㶼����ǰһ����ѹ�����
// Ȼ������CPU��ת��Ȧ�͸պ��ܴﵽ���辫�ȡ���������ÿ���õ���ʱ��͵�ǰʱ��Ĳ�ֵ����˯��ʱ�䣬
// �����趨����ʱ�䵽��ȡ��ǰʱ��֮�仹����һЩ��������һ��ʱ�䣬���¼����˯��ʱ����趨�ļ�ʱ��
// ���ʱ����������һ���루���ڳ�����ȡ���ģ�������CPU�ͻ���תһ���룬Ϊ�˼�����һ����Ŀ�ת��
// ����һ������ֵ��
const EzInt64 COMPENSATION_TIME = 40;			// 40 microseconds

struct TimerThreadParam
{
	MiniHeapTimer*	pTimerService;
	HANDLE			hEvent;

	TimerThreadParam()
		: pTimerService(NULL)
		, hEvent(NULL)
	{}
};

enum APCEvent
{
	kAddTimer = 1,
	kKillTimer,
	kExitThread
};

struct APCProcParam : public EzHeapOper
{
	MiniHeapTimer*	pTimerService;
	APCEvent		event;
	EzUInt			uTimerId;
	EzUInt			uElapse;

	APCProcParam()
		: pTimerService(NULL)
		, event(kAddTimer)
		, uTimerId(0)
		, uElapse(0)
	{}
};

//-------------------------------------------------------------------------------
// MiniHeapTimer
//-------------------------------------------------------------------------------
MiniHeapTimer::MiniHeapTimer()
	: m_stateFlags(0)
	, m_hTimerThread(INVALID_HANDLE_VALUE)
	, m_bQuit(false)
	, m_nStartCounter(0)
	, m_nFrequency(0)
{

}

MiniHeapTimer::~MiniHeapTimer()
{
	stop();
	unInit();
}

SVCErrorCode MiniHeapTimer::init()
{
	if (m_stateFlags & kInited)
		return eOk;

	m_stateFlags |= kInited;
	return eOk;
}

SVCErrorCode MiniHeapTimer::unInit()
{
	if (!(m_stateFlags & kInited))
		return eOk;

	m_stateFlags &= ~kInited;
	return eOk;
}

SVCErrorCode MiniHeapTimer::start()
{
	if (!(m_stateFlags & kInited))
		return eNotInitialized;
	if (m_stateFlags & kRunning)
		return eOk;

	HANDLE hWaitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!EzVerify(hWaitEvent != NULL))
		return eSystemError;

	TimerThreadParam param;
	param.pTimerService = this;
	param.hEvent = hWaitEvent;

	m_bQuit = false;
	m_hTimerThread = (HANDLE)::_beginthreadex(NULL, 0, timerThreadProc, &param, 0, NULL);
	if (!EzVerify(m_hTimerThread != NULL))
	{
		::CloseHandle(hWaitEvent);
		EzLogError(_T("create timer thread fail!\n"));
		return eSystemError;
	}

	::WaitForSingleObject(hWaitEvent, INFINITE);
	::CloseHandle(hWaitEvent);
	hWaitEvent = NULL;

	m_stateFlags |= kRunning;
	return eOk;
}

SVCErrorCode MiniHeapTimer::stop()
{
	if (!(m_stateFlags & kRunning))
		return eOk;

	m_bQuit = true;
	// Wakeup the timer thread first
	APCProcParam* pParam = new APCProcParam();
	pParam->pTimerService = this;
	pParam->event = kExitThread;
	::QueueUserAPC(timerAPCProc, m_hTimerThread, (ULONG_PTR)pParam);
	::WaitForSingleObject(m_hTimerThread, INFINITE);
	::CloseHandle(m_hTimerThread);
	m_hTimerThread = NULL;

	m_timerHeap.clear();

	m_stateFlags &= ~kRunning;
	return eOk;
}

bool MiniHeapTimer::setTimer(EzUInt uTimerId, EzUInt uElapse)
{
	if (!(m_stateFlags & kRunning))
		return false;
	if (0 == uTimerId || 0 == uElapse)
		return false;

	APCProcParam* pParam = new APCProcParam();
	pParam->pTimerService = this;
	pParam->event = kAddTimer;
	pParam->uTimerId = uTimerId;
	pParam->uElapse = uElapse;

	::QueueUserAPC(timerAPCProc, m_hTimerThread, (ULONG_PTR)pParam);
	return true;
}

bool MiniHeapTimer::killTimer(EzUInt uTimerId)
{
	if (!(m_stateFlags & kRunning))
		return false;
	if (0 == uTimerId)
		return false;

	APCProcParam* pParam = new APCProcParam();
	pParam->pTimerService = this;
	pParam->event = kKillTimer;
	pParam->uTimerId = uTimerId;

	::QueueUserAPC(timerAPCProc, m_hTimerThread, (ULONG_PTR)pParam);
	return true;
}

void MiniHeapTimer::dump(EzDumpContext& dc) const
{

}

unsigned __stdcall MiniHeapTimer::timerThreadProc(void* pParam)
{
	TimerThreadParam* pThreadParam = (TimerThreadParam*)pParam;
	EzAssert(pThreadParam != NULL);

	// According to MSDN: it's best to keep the thread on a single processor.
	DWORD_PTR dwOld = ::SetThreadAffinityMask(::GetCurrentThread(), 1);

	MiniHeapTimer* pTimerService = pThreadParam->pTimerService;
	pTimerService->timerThreadInit();
	::SetEvent(pThreadParam->hEvent);

	TIMECAPS timeCaps;
	UINT iPeriodMin = 1;
	if(MMSYSERR_NOERROR == ::timeGetDevCaps(&timeCaps, sizeof(timeCaps)))
		iPeriodMin = timeCaps.wPeriodMin;

	::timeBeginPeriod(iPeriodMin);

	while (!pTimerService->m_bQuit)
		pTimerService->timerThreadRun();

	::timeEndPeriod(iPeriodMin);

	return 0;
}

void MiniHeapTimer::notifyTimerMsg(EzUInt uTimerId)
{
	for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
	{
		ITimerServiceEventHandler* pEventHandler = m_EventHandlers[i];
		if (pEventHandler)
			pEventHandler->onTimerMessage(uTimerId);
	}
}

void MiniHeapTimer::timerThreadInit()
{
	LARGE_INTEGER liPerfFreq;
	::QueryPerformanceFrequency(&liPerfFreq);
	m_nFrequency = liPerfFreq.QuadPart;

	LARGE_INTEGER liPerfStart;
	::QueryPerformanceCounter(&liPerfStart);
	m_nStartCounter = liPerfStart.QuadPart;
}

void MiniHeapTimer::timerThreadRun()
{
	const HeapTimerNode* pNode = m_timerHeap.peek();
	if (NULL == pNode)
	{
		::SleepEx(INFINITE, TRUE);
	}
	else
	{
		EzInt64 nNow = getNowElapsedTime();
		if (nNow >= pNode->expires)
		{
			HeapTimerNode node;
			EzVerify(m_timerHeap.poll(node));
			m_timerHeap.addTimer(node.timerId, node.uElapse, calcTimerExpiresTime(node.uElapse));		// Re add

			notifyTimerMsg(node.timerId);
		}
		else
		{
			DWORD dwSleepTime = (DWORD)((pNode->expires - nNow + COMPENSATION_TIME) / 1000);		// microseconds -> milliseconds
			::SleepEx(dwSleepTime, TRUE);
		}
	}
}

EzInt64 MiniHeapTimer::getNowElapsedTime() const
{
	LARGE_INTEGER liPerfNow;
	::QueryPerformanceCounter(&liPerfNow);
	// seconds -> milliseconds -> microseconds
	return (EzInt64)((liPerfNow.QuadPart - m_nStartCounter) / (double)m_nFrequency * 1000 * 1000);
}

EzInt64 MiniHeapTimer::calcTimerExpiresTime(EzUInt uElapse) const
{
	// convert uElapse from milliseconds to microseconds
	return getNowElapsedTime() + uElapse * 1000;
}

void CALLBACK MiniHeapTimer::timerAPCProc(ULONG_PTR dwParam)
{
	APCProcParam* pAPCParam = (APCProcParam*)dwParam;

	switch (pAPCParam->event)
	{
	case kAddTimer:
		pAPCParam->pTimerService->addTimerToHeap(pAPCParam->uTimerId, pAPCParam->uElapse);
		break;
	case kKillTimer:
		pAPCParam->pTimerService->removeTimerFromHeap(pAPCParam->uTimerId);
		break;
	case kExitThread:		// nothing to do
		break;
	default:
		break;
	}

	delete pAPCParam;
}

bool MiniHeapTimer::addTimerToHeap(EzUInt uTimerId, EzUInt uElapse)
{
	return m_timerHeap.addTimer(uTimerId, uElapse, calcTimerExpiresTime(uElapse));
}

bool MiniHeapTimer::removeTimerFromHeap(EzUInt uTimerId)
{
	return m_timerHeap.removeTimer(uTimerId);
}

#include "stdafx.h"
#include "TestTimer.h"


TestTimer::TestTimer()
	: m_pTimerService(NULL)
	, m_bRunning(false)
{
	m_pTimerService = createTimerService();
	if (m_pTimerService != NULL)
		m_pTimerService->addEventHandler(this);
}

TestTimer::~TestTimer()
{
	if (m_pTimerService != NULL)
		releaseTimerService(m_pTimerService);
}

bool TestTimer::start()
{
	if (m_bRunning)
		return true;

	if (NULL == m_pTimerService)
		return false;

	SVCErrorCode ec = eOk;
	ec = m_pTimerService->init();
	if (ec != eOk)
		return false;

	ec = m_pTimerService->start();
	if (ec != eOk)
		return false;

	m_bRunning = true;
	return true;
}

bool TestTimer::stop()
{
	if (!m_bRunning)
		return false;

	SVCErrorCode ec = eOk;
	ec = m_pTimerService->stop();
	EzAssert(ec == eOk);

	ec = m_pTimerService->unInit();
	EzAssert(ec == eOk);

	m_bRunning = false;
	return true;
}

void TestTimer::onTimerMessage(EzUInt uTimerId)
{
	switch (uTimerId)
	{
	case 100:
		{
			static EzStopwatch watch;
			static bool bFirst = true;

			if (bFirst)
			{
				watch.start();
				bFirst = false;
			}
			else
			{
				EzTrace(_T("------------Timer100, Time: %I64d-------------"), watch.stop());
				watch.start();
			}
		}
		break;
	case 101:
		{
			static EzStopwatch watch;
			static bool bFirst = true;

			if (bFirst)
			{
				watch.start();
				bFirst = false;
			}
			else
			{
				EzTrace(_T("------------Timer101, Time: %I64d-------------"), watch.stop());
				watch.start();
			}
		}
		break;
	default:
		break;
	}
}

bool TestTimer::setTimer(EzUInt uTimerId, EzUInt uElapse)
{
	if (!m_bRunning)
		return false;

	EzAssert(m_pTimerService != NULL);
	return m_pTimerService->setTimer(uTimerId, uElapse);
}

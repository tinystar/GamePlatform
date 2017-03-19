#include "xEzProfile.h"
#include <windows.h>
#include "xEzDebug.h"
#include "xEzMacro.h"


class EzStopwatchImp
{
public:
	EzStopwatchImp() { ::QueryPerformanceFrequency(&m_liPerfFreq); }

	void start() { ::QueryPerformanceCounter(&m_liPerfStart); }

	EzInt64 stop() const
	{
		LARGE_INTEGER liPerfNow;
		::QueryPerformanceCounter(&liPerfNow);

		return ((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart;
	}

private:
	LARGE_INTEGER	m_liPerfFreq;
	LARGE_INTEGER	m_liPerfStart;
};


//------------------------------------------------------
// EzStopwatch
//------------------------------------------------------
EzStopwatch::EzStopwatch()
{
	m_pImpWatch = new EzStopwatchImp;
	EzAssert(m_pImpWatch);
}

EzStopwatch::~EzStopwatch()
{
	EZ_SAFE_DELETE(m_pImpWatch);
}

void EzStopwatch::start() const
{
	if (m_pImpWatch)
		m_pImpWatch->start();
}

EzInt64 EzStopwatch::stop() const
{
	if (m_pImpWatch)
		return m_pImpWatch->stop();
	return 0;
}

#include "TimerService.h"
#include <iostream>

using namespace std;


class TestTimerService : public ITimerServiceEventHandler
{
public:
	TestTimerService()
		: m_pTimerService(NULL)
	{
		m_pTimerService = createTimerService();
		if (m_pTimerService != NULL)
			m_pTimerService->addEventHandler(this);
	}

	~TestTimerService()
	{
		if (m_pTimerService != NULL)
			releaseTimerService(m_pTimerService);
	}

	void start()
	{
		m_pTimerService->init();
		m_pTimerService->start();
	}

	void stop()
	{
		m_pTimerService->stop();
		m_pTimerService->unInit();
	}

	bool setTimer(EzUInt uTimerId, EzUInt uElapse)
	{
		return m_pTimerService->setTimer(uTimerId, uElapse);
	}

protected:
	virtual void onTimerMessage(EzUInt uTimerId);

protected:
	TimerService* m_pTimerService;
};

EzStopwatch watch;
volatile int tickCount = 0;

void TestTimerService::onTimerMessage(EzUInt uTimerId)
{
	double dDuration = watch.stop();
	//watch.start();
	cout << uTimerId << " Timer, Duration: " << dDuration << endl;

	if (tickCount++ == 30)
	{
		for (EzUInt i = 1; i <= 8; ++i)
			m_pTimerService->killTimer(i);

		m_pTimerService->killTimer(10);
	}
	if (tickCount == 5)
	{
		m_pTimerService->setTimer(10, 300);
	}
	if (tickCount == 7)
	{
		//m_pTimerService->setTimer(4, 150);
		m_pTimerService->setTimer(6, 60);
	}
	if (tickCount == 15)
	{
		m_pTimerService->killTimer(5);
	}
}


void TimerServiceTestEntry()
{
	TestTimerService timer;
	timer.start();

	EzUInt uId = 0;
	timer.setTimer(++uId, 530);
	timer.setTimer(++uId, 170);
	timer.setTimer(++uId, 780);
	timer.setTimer(++uId, 90);
	timer.setTimer(++uId, 450);
	timer.setTimer(++uId, 650);
	timer.setTimer(++uId, 870);
	timer.setTimer(++uId, 230);
	watch.start();

	int n;
	cin >> n;
	timer.stop();
}
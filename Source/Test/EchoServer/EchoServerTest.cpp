#include "stdafx.h"
#include "EchoServerTest.h"

using namespace EzTime;

#define SEND_TIME_TIMER_ID		1
#define SEND_TIMER_ELAPSE		100

void EchoServer::onTcpClientConnectMsg(ClientId id)
{

}

void EchoServer::onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize)
{
	m_pTcpService->sendData(id, pPackage, nSize);
}

void EchoServer::onTcpClientCloseMsg(ClientId id)
{

}

void EchoServer::onTimerMsg(EzUInt uTimerId)
{

}


//--------------------------------------------------------------
// TimeServer
//--------------------------------------------------------------
void TimeServer::onTcpClientConnectMsg(ClientId id)
{
	sendCurrentTime(id);
}

void TimeServer::onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize)
{

}

void TimeServer::onTcpClientCloseMsg(ClientId id)
{

}

void TimeServer::onTimerMsg(EzUInt uTimerId)
{
	if (SEND_TIME_TIMER_ID == uTimerId)
	{
		if (m_pTcpService->getClientCount() <= 0)
			return;

		//ClientIdIterator* pIter = m_pTcpService->newIterator();
		//if (pIter != NULL)
		//{
		//	while (!pIter->isDone())
		//	{
		//		sendCurrentTime(pIter->getClientId());
		//		pIter->step();
		//	}
		//	delete pIter;
		//}
		int h, m, s;
		getTimeCurrent(h, m, s);

		TimeMsg msg;
		msg.nHour = h;
		msg.nMin = m;
		msg.nSec = s;

		sendDataToAll(&msg, sizeof(msg));
	}
}

bool TimeServer::sendCurrentTime(ClientId id)
{
	if (!id.isValid())
		return false;

	int h, m, s;
	getTimeCurrent(h, m, s);

	TimeMsg msg;
	msg.nHour = h;
	msg.nMin = m;
	msg.nSec = s;
	return sendData(id, &msg, sizeof(msg));
}

bool TimeServer::onStart()
{
	setTimer(SEND_TIME_TIMER_ID, SEND_TIMER_ELAPSE);
	return true;
}

bool TimeServer::onStop()
{
	killTimer(SEND_TIME_TIMER_ID);
	return true;
}

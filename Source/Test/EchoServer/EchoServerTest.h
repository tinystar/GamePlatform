#ifndef __ECHO_SERVER_TEST__
#define __ECHO_SERVER_TEST__

#include "ServerTemplate.h"
#include "ServiceTypes.h"

#define ECHO_SERVER_PORT		5010
#define MAX_ECHOMSG_SIZE		4096
#define TIME_SERVER_PORT		5015
#define MAX_TIMEMSG_SIZE		100

class EchoServer : public ServerTemplate
{
protected:
	virtual void onTcpClientConnectMsg(ClientId id);
	virtual void onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize);
	virtual void onTcpClientCloseMsg(ClientId id);

	virtual void onTimerMsg(EzUInt uTimerId);
};

struct TimeMsg
{
	CSINT	nHour;
	CSINT	nMin;
	CSINT	nSec;
};

class TimeServer : public ServerTemplate
{
protected:
	virtual void onTcpClientConnectMsg(ClientId id);
	virtual void onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize);
	virtual void onTcpClientCloseMsg(ClientId id);

	virtual void onTimerMsg(EzUInt uTimerId);

	virtual bool onStart();
	virtual bool onStop();

protected:
	bool sendCurrentTime(ClientId id) const;
};

#endif // __ECHO_SERVER_TEST__
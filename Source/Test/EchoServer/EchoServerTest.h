#ifndef __ECHO_SERVER_TEST__
#define __ECHO_SERVER_TEST__

#include "ServerTemplate.h"
#include "ServiceTypes.h"
#include "IServerUIObserver.h"

#define ECHO_SERVER_PORT		5010
#define MAX_ECHOMSG_SIZE		4096
#define TIME_SERVER_PORT		5015
#define MAX_TIMEMSG_SIZE		100

class EchoServer : public ServerTemplate
{
public:
	EchoServer()
		: m_pUIObserver(NULL)
	{}

public:
	void registerUIObserver(IServerUIObserver* pObserver) { m_pUIObserver = pObserver; }

protected:
	virtual void onTcpClientConnectMsg(ClientId id);
	virtual void onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize);
	virtual void onTcpClientCloseMsg(ClientId id);

	virtual void onTimerMsg(EzUInt uTimerId);

	virtual void onUserItemMsg(int itemId, void* pData, size_t nSize){}

private:
	IServerUIObserver* m_pUIObserver;
};

#pragma pack(push, 1)

struct TimeMsg
{
	CSINT	nHour;
	CSINT	nMin;
	CSINT	nSec;
};

#pragma pack(pop)

class TimeServer : public ServerTemplate
{
protected:
	virtual void onTcpClientConnectMsg(ClientId id);
	virtual void onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize);
	virtual void onTcpClientCloseMsg(ClientId id);

	virtual void onTimerMsg(EzUInt uTimerId);

	virtual void onUserItemMsg(int itemId, void* pData, size_t nSize){}

	virtual bool onStart();
	virtual bool onStop();

protected:
	bool sendCurrentTime(ClientId id);
};

#endif // __ECHO_SERVER_TEST__
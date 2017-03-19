#ifndef __TEST_SERVER_H__
#define __TEST_SERVER_H__

#include "TcpService.h"
#include <vector>
#include <fstream>

class TestServer : public ITcpServiceEventHandler
{
public:
	TestServer();
	~TestServer();

	bool start();
	bool stop();

	void test();

public:
	virtual void onClientConnected(ClientId id);
	virtual bool onPackageReceived(ClientId id, void* pPackage, size_t nSize);
	virtual bool onClientClosed(ClientId id);

protected:
	TcpService*				m_pTcpService;
	bool					m_bRunning;
	char					m_szFileName[260];
	std::ofstream			m_ofile;
};

#endif
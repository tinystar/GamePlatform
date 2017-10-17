/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/8/13
/************************************************************************/
#ifndef __GATE_SERVER_MGR_H__
#define __GATE_SERVER_MGR_H__

#include "GateServer.h"

class GateServerMgr
{
public:
	GateServerMgr();

public:
	SVCErrorCode initServer();
	SVCErrorCode unInitServer();
	SVCErrorCode startServer();
	SVCErrorCode stopServer();

	unsigned short getPort() const { return m_tcpConfig.sPort; }
	const char* getVersion() const { return m_server.getVersion(); }
	const char* getUpdUrl() const { return m_server.getUpdUrl(); }

	GateServer& getServer() { return m_server; }

protected:
	bool loadConfig();

protected:
	GateServer		m_server;

	bool			m_bDebugMode;
	TcpConfig		m_tcpConfig;
};

#endif // __GATE_SERVER_MGR_H__

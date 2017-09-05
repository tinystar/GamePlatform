/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/4
/************************************************************************/
#ifndef __DB_SERVER_MGR_H__
#define __DB_SERVER_MGR_H__

#include "DBServer.h"
#include <string>

class DBServerMgr
{
public:
	DBServerMgr();

public:
	SVCErrorCode initServer();
	SVCErrorCode unInitServer();
	SVCErrorCode startServer();
	SVCErrorCode stopServer();

protected:
	bool loadConfig();

protected:
	DBServer		m_server;

	bool			m_bDebugMode;
	LogLevel		m_logLevel;
	TcpConfig		m_tcpConfig;
};

#endif // __DB_SERVER_MGR_H__

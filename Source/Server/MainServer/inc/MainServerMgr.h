/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/9
/************************************************************************/
#ifndef __MAIN_SERVER_MGR_H__
#define __MAIN_SERVER_MGR_H__

#include "MainServer.h"
#include <string>

class MainServerMgr
{
public:
	MainServerMgr();

public:
	SVCErrorCode initServer();
	SVCErrorCode unInitServer();
	SVCErrorCode startServer();
	SVCErrorCode stopServer();

	bool loadConfig();

	unsigned short getPort() { return m_tcpConfig.sPort; }
	void setPort(unsigned short sPort) { m_tcpConfig.sPort = sPort; }
	
	void setGateSvrAddr(const char* pszAddr, unsigned short sPort);
	bool getGateSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort);

	void setDBSvrAddr(const char* pszAddr, unsigned short sPort);
	bool getDBSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort);

protected:
	MainServer			m_server;

	bool				m_bDebugMode;
	LogLevel			m_logLevel;
	TcpConfig			m_tcpConfig;
	char				m_szGateAddr[20];
	char				m_szDBAddr[20];
	unsigned short		m_sGatePort;
	unsigned short		m_sDBPort;
};

#endif // __MAIN_SERVER_MGR_H__

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

	unsigned short getPort() const { return m_tcpConfig.sPort; }
	void setPort(unsigned short sPort) { m_tcpConfig.sPort = sPort; }
	
	void setGateSvrAddr(const char* pszAddr, unsigned short sPort);
	bool getGateSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort) const;

	void setDBSvrAddr(const char* pszAddr, unsigned short sPort);
	bool getDBSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort) const;

	void setMaxUser(unsigned int uMax) { return m_server.setMaxUser(uMax); }
	unsigned int getMaxUser() const { return m_server.getMaxUser(); }

protected:
	MainServer			m_server;

	bool				m_bDebugMode;
	TcpConfig			m_tcpConfig;
};

#endif // __MAIN_SERVER_MGR_H__

/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/9
/************************************************************************/
#ifndef __MAIN_SERVER_H__
#define __MAIN_SERVER_H__

#include "BaseGameServer.h"

class MainServer : public BaseGameServer
{
public:
	MainServer();
	virtual ~MainServer();

public:
	void setGateSvrAddr(const char* pszAddr, unsigned short sPort);
	void setDBSvrAddr(const char* pszAddr, unsigned short sPort);

protected:
	virtual bool onInit();
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();

protected:
	char				m_szGateAddr[20];
	char				m_szDBAddr[20];
	unsigned short		m_sGatePort;
	unsigned short		m_sDBPort;
};

#endif // __MAIN_SERVER_H__

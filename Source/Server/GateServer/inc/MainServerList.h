/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/10/2
/************************************************************************/
#ifndef __MAIN_SERVER_LIST_H__
#define __MAIN_SERVER_LIST_H__

#include "ClientId.h"
#include <set>


struct MainSvrNode
{
	ClientId			svrId;
	char				szMainAddr[20];
	char				szMainName[64];
	unsigned short		sMainPort;
	unsigned int		uMaxUser;
	unsigned int		uOnlineUser;

	MainSvrNode()
		: sMainPort(0)
		, uMaxUser(0)
		, uOnlineUser(0)
	{
		::memset(szMainAddr, 0, sizeof(szMainAddr));
		::memset(szMainName, 0, sizeof(szMainName));
	}
};

class MainServerList
{
public:
	MainServerList();
	~MainServerList();

public:
	void addServer(const MainSvrNode& svr);
	void removeServer(const ClientId& id);
	void addServerUser(const ClientId& id);
	void removeServerUser(const ClientId& id);

	const MainSvrNode* chooseServer() const;

private:
	EzArray<MainSvrNode>	m_svrList;
};

#endif // __MAIN_SERVER_LIST_H__

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
	unsigned short		sMainPort;
	unsigned int		uMaxUser;

	std::set<EzUInt32>	onlineUserIds;

	MainSvrNode()
		: sMainPort(0)
		, uMaxUser(0)
	{
		::memset(szMainAddr, 0, sizeof(szMainAddr));
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
	void addServerUser(const ClientId& id, EzUInt32 userId);
	void removeServerUser(const ClientId& id, EzUInt32 userId);
	const MainSvrNode* getServerByUserId(EzUInt32 userId);

	const MainSvrNode* chooseServer() const;

private:
	EzArray<MainSvrNode>	m_svrList;
};

#endif // __MAIN_SERVER_LIST_H__

/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/4
/************************************************************************/
#ifndef __DB_SERVER_H__
#define __DB_SERVER_H__

#include <afxdb.h>
#include "BaseGameServer.h"
#include "IDBServerUIObserver.h"

class DBServer : public BaseGameServer
{
public:
	DBServer();
	virtual ~DBServer();

public:
	bool setDSN(const char* pszDSN);

	void registerUIObserver(IDBServerUIObserver* pObserver) { m_pUIObserver = pObserver; }

protected:
	virtual bool onInit(const ServerInitConfig& serverConfig);
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();

protected:
	void onQueryGameKinds(ClientId id, void* pData, size_t nDataLen);
	void onQueryGamePlaces(ClientId id, void* pData, size_t nDataLen);
	void onQueryGameRooms(ClientId id, void* pData, size_t nDataLen);

protected:
	static NetMsgMapEntry s_msgMapArray[];

	CDatabase				m_database;
	char					m_szDSN[256];
	IDBServerUIObserver*	m_pUIObserver;
};

#endif // __DB_SERVER_H__

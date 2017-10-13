/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/8/9
/************************************************************************/
#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__

#include "BaseGameServer.h"
#include "MainServerList.h"

class GateServer : public BaseGameServer
{
public:
	GateServer();
	virtual ~GateServer();

public:
	const char* getVersion() const { return m_szVersion; }
	bool setVersion(const char* pszVer);

	const char* getUpdUrl() const { return m_szUpdUrl; }
	bool setUpdUrl(const char* pszUrl);

protected:
	virtual bool onInit(const ServerInitConfig&);
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();

protected:
	virtual void onTcpClientCloseMsg(ClientId id);

protected:
	void onRequestConfig(ClientId id, void* pData, size_t nDataLen);
	void onRequestMainAddr(ClientId id, void* pData, size_t nDataLen);

	void onMainConnect(ClientId id, void* pData, size_t nDataLen);

protected:
	static NetMsgMapEntry s_msgMapArray[];

	MainServerList		m_mainSvrList;
	char				m_szVersion[16];
	char				m_szUpdUrl[256];
};

#endif // __GATE_SERVER_H__

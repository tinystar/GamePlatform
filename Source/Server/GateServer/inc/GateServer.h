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

class GateServer : public BaseGameServer
{
public:
	GateServer();
	virtual ~GateServer();

protected:
	virtual bool onInit();
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();

protected:
	virtual void onDefaultMsgHandler(ClientId id, GameMsgHeader* pHeader, void* pData, size_t nDataLen);
};

#endif // __GATE_SERVER_H__

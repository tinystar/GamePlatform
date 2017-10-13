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

#include "BaseGameServer.h"

class DBServer : public BaseGameServer
{
public:
	DBServer();
	virtual ~DBServer();

protected:
	virtual bool onInit(const ServerInitConfig&);
	virtual bool onUninit();
	virtual bool onStart();
	virtual bool onStop();
};

#endif // __DB_SERVER_H__

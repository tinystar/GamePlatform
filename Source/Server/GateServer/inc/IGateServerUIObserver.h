/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/10/15
/************************************************************************/
#ifndef __IGATESERVER_UI_OBSERVER_H__
#define __IGATESERVER_UI_OBSERVER_H__

#include "ClientId.h"
#include "MainServerList.h"

class IGateServerUIObserver
{
public:
	virtual void onUIMainServerAdded(ClientId id, const MainSvrNode& svrNode) = 0;
	virtual void onUIMainServerRemoved(ClientId id) = 0;
	virtual void onUIMainUserLogin(ClientId id) = 0;
	virtual void onUIMainUserLogout(ClientId id) = 0;
};

#endif // __IGATESERVER_UI_OBSERVER_H__

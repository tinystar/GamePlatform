/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/10/15
/************************************************************************/
#ifndef __IMAINSERVER_UI_OBSERVER_H__
#define __IMAINSERVER_UI_OBSERVER_H__

#include "ClientId.h"

class IMainServerUIObserver
{
public:
	virtual void onUIConnToGateSuccess() = 0;
	virtual void onUIConnToGateFail(int nErrCode) = 0;
	virtual void onUIConnToGateClosed() = 0;

	virtual void onUIConnToDBSuccess() = 0;
	virtual void onUIConnToDBFail(int nErrCode) = 0;
	virtual void onUIConnToDBClosed() = 0;
};

#endif // __IMAINSERVER_UI_OBSERVER_H__

/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/21
/************************************************************************/
#ifndef __ICLIENT_SESSION_MGR_H__
#define __ICLIENT_SESSION_MGR_H__

#include "CoreService.h"

class ClientContext;

struct IClientSessionMgr
{
	virtual bool OnErrorOccurredNotify(ClientContext* pClient, SVCErrorCode errType, int iErrCode) = 0;
	virtual bool OnPackageRecvedNotify(ClientContext* pClient, void* pPackage, size_t nSize) = 0;
};

#endif // __ICLIENT_SESSION_MGR_H__

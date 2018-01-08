/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/12/17
/************************************************************************/
#ifndef __IDBSERVER_UI_OBSERVER_H__
#define __IDBSERVER_UI_OBSERVER_H__

#include <tchar.h>

class IDBServerUIObserver
{
public:
	virtual void onUIDatabaseOpen(bool bSucc, const TCHAR* pszMsg) = 0;
	virtual void onUIDatabaseClose() = 0;
};

#endif // __IDBSERVER_UI_OBSERVER_H__

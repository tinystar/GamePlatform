/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/2/25
/************************************************************************/
#ifndef __CLIENT_ID_ITERATOR_H__
#define __CLIENT_ID_ITERATOR_H__

#include "ClientId.h"


class ClientIdIterator
{
public:
	virtual ~ClientIdIterator() {}

	virtual bool start() = 0;
	virtual bool step() = 0;
	virtual bool isDone() = 0;

	virtual ClientId getClientId() = 0;
};

#endif // __CLIENT_ID_ITERATOR_H__
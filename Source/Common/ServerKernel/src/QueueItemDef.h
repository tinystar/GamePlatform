/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/4/29
/************************************************************************/
#ifndef __QUEUE_ITEM_DEF_H__
#define __QUEUE_ITEM_DEF_H__

#include "xEzUtil.h"
#include "ClientId.h"

enum QueueItemType
{
	kTypeUnknown = 0,
	kTypeClientConn,
	kTypePackageRecv,
	kTypeClientClose,
	kTypeTimer
};

class QueueItemHead : public EzHeapOper
{
public:
	EzUInt32		uItemType;

	QueueItemHead()
		: uItemType(kTypeUnknown)
	{}

	virtual ~QueueItemHead() {}
};

class QueueClientConnMsg : public QueueItemHead
{
public:
	ClientId		clientId;
};

class QueuePackageRecvMsg : public QueueItemHead
{
public:
	ClientId		clientId;
	EzUInt32		uDataSize;
	void*			pRecvData;

	QueuePackageRecvMsg()
		: uDataSize(0)
		, pRecvData(NULL)
	{}

	~QueuePackageRecvMsg()
	{
		if (pRecvData != NULL)
		{
			delete[] pRecvData;
			pRecvData = NULL;
		}
	}
};

class QueueClientCloseMsg : public QueueItemHead
{
public:
	ClientId		clientId;
};

class QueueTimerMsg : public QueueItemHead
{
public:
	EzUInt			uTimerId;
};

#endif // __QUEUE_ITEM_DEF_H__
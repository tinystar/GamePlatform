/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/3/17
/************************************************************************/
#ifndef __NET_MSGMAP_DEFS_H__
#define __NET_MSGMAP_DEFS_H__

#include "ClientId.h"

class BaseGameServer;

typedef void (BaseGameServer::*NetMsgHandler)(ClientId, void*, size_t);

struct NetMsgEntry
{
	CSUINT16		uMainId;
	CSUINT16		uSubId;
	NetMsgHandler	pHandler;
};

struct NetMsgTable
{
	const NetMsgTable* (*pfnGetBaseTable)();
	const NetMsgEntry* pEntries;
};

#define DECLARE_NETMSG_TABLE() \
protected: \
	static const NetMsgTable* GetThisNetMsgTable(); \
	virtual const NetMsgTable* GetNetMsgTable() const;

#define BEGIN_NETMSG_TABLE(theClass, baseClass) \
	const NetMsgTable* theClass::GetNetMsgTable() const \
	{ \
		return GetThisNetMsgTable(); \
	} \
	const NetMsgTable* theClass::GetThisNetMsgTable() \
	{ \
		typedef theClass ThisClass; \
		typedef baseClass TheBaseClass; \
		static const NetMsgEntry _messageEntries[] = \
		{

#define END_NETMSG_TABLE() \
			{0, 0, (NetMsgHandler)0 } \
		}; \
		static const NetMsgTable messageTable = \
		{ &TheBaseClass::GetThisNetMsgTable, &_messageEntries[0] }; \
		return &messageTable; \
	}

#define ON_NET_MESSAGE(MainMsgId, SubMsgId, MemberMsgFunc) \
	{ MainMsgId, SubMsgId, static_cast<NetMsgHandler>(MemberMsgFunc) },

#endif // __NET_MSGMAP_DEFS_H__

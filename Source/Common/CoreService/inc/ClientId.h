/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/16
/************************************************************************/
#ifndef __CLIENT_ID_H__
#define __CLIENT_ID_H__

#include "CoreService.h"
#include "ServiceTypes.h"


class ClientContext;

class SVC_DLL_SPEC ClientId
{
public:
	ClientId()
		: m_pClientCtx(NULL)
	{}

	ClientId(ClientContext* pClientCtx)
		: m_pClientCtx(pClientCtx)
	{}

	static const ClientId kNull;

	bool isNull() const;
	void setNull();

	bool isValid() const;

	SOCKADDR_IN getAddress() const;

	void setUserData(void* pData);
	void* getUserData() const;

	/**
	* The ClientContext structure is cached by a pool, that is it will be reused instead of be released
	* after the client is closed.So we need a mechanism to uniquely identify a client connection.I solve
	* this by generating an unique number and attach it to the ClientContext.You can use this number to
	* identify the client in a server.When the client is closed the unique number is reset to zero.
	*/
	CSHANDLE getUniqueHandle() const;

	// for debug
	void dump(EzDumpContext& dc) const;

	ClientId& operator= (const ClientId& other);
	ClientId& operator= (const ClientContext* pClientCtx);

	operator ClientContext*() const;

	bool operator< (const ClientId& rhs) const;
	bool operator> (const ClientId& rhs) const;
	bool operator== (const ClientId& rhs) const;
	bool operator!= (const ClientId& rhs) const;

private:
	ClientContext* m_pClientCtx;
};

inline bool ClientId::isNull() const
{
	return m_pClientCtx == NULL;
}

inline void ClientId::setNull()
{
	m_pClientCtx = NULL;
}

inline ClientId& ClientId::operator= (const ClientId& other)
{
	m_pClientCtx = other.m_pClientCtx;
	return *this;
}

inline ClientId& ClientId::operator= (const ClientContext* pClientCtx)
{
	m_pClientCtx = const_cast<ClientContext*>(pClientCtx);
	return *this;
}

inline ClientId::operator ClientContext*() const
{
	return m_pClientCtx;
}

#endif // __CLIENT_ID_H__
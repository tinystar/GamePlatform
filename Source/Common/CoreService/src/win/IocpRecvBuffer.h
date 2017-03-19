/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/23
/************************************************************************/
#ifndef __IOCP_RECV_BUFFER_H__
#define __IOCP_RECV_BUFFER_H__

#include "xEzUtil.h"

class ClientContext;
struct PER_IO_CONTEXT;
struct IClientSessionMgr;


class IocpRecvBuffer
{
public:
	IocpRecvBuffer()
		: m_nMaxPkgSize(0)
		, m_nBufferSize(0)
		, m_nRecvedSize(0)
		, m_pBuffer(NULL)
	{
	}

	~IocpRecvBuffer()
	{
		EZ_SAFE_DELETE_ARRAY(m_pBuffer);
	}

	bool init(size_t nMaxPkgSize);

	bool asyncRecv(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, IClientSessionMgr* pClientMgr);

	bool onRecvCompleted(ClientContext* pClient, IClientSessionMgr* pClientMgr);

	size_t getUsedSize() const { return m_nRecvedSize; }

	void reset();

protected:
	size_t calcBufferSize(size_t nPkgSize);
	int safeRecv(ClientContext* pClient, int iRecvSize, IClientSessionMgr* pClientMgr);

private:
	size_t			m_nMaxPkgSize;
	size_t			m_nBufferSize;
	size_t			m_nRecvedSize;
	char*			m_pBuffer;
};

inline void IocpRecvBuffer::reset()
{
	m_nRecvedSize = 0;
}

#endif // __IOCP_RECV_BUFFER_H__

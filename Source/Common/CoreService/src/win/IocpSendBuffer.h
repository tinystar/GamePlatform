/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/29
/************************************************************************/
#ifndef __IOCP_SEND_BUFFER_H__
#define __IOCP_SEND_BUFFER_H__

#include "xEzUtil.h"

class ClientContext;
struct PER_IO_CONTEXT;
struct IClientSessionMgr;


/**
 * ring buffer to improve performance
 */
class IocpSendBuffer
{
public:
	IocpSendBuffer()
		: m_nMaxPkgSize(0)
		, m_nBufferSize(0)
		, m_nUsedSize(0)
		, m_nHeadPos(0)
		, m_nTailPos(0)
		, m_pBuffer(NULL)
		, m_bSending(false)
	{
	}

	~IocpSendBuffer()
	{
		EZ_SAFE_DELETE_ARRAY(m_pBuffer);
	}

	bool init(size_t nMaxPkgSize);

	bool asyncSend(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, IClientSessionMgr* pClientMgr, void* pData, size_t nDataLen);

	bool onSendCompleted(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, size_t nSize, IClientSessionMgr* pClientMgr);

	bool isEmpty() const { return m_nUsedSize == 0; }
	bool isFull() const { return m_nUsedSize == m_nBufferSize; }

	size_t getBufferSize() const { return m_nBufferSize; }
	size_t getUsedSize() const { return m_nUsedSize; }
	size_t getFreeSize() const { return m_nBufferSize - m_nUsedSize; }

	void reset();

protected:
	size_t calcBufferSize(size_t nPkgSize);

	bool doSend(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, IClientSessionMgr* pClientMgr);

	size_t writeBuffer(void* pData, size_t nDataLen);

private:
	size_t			m_nMaxPkgSize;
	size_t			m_nBufferSize;
	size_t			m_nUsedSize;
	size_t			m_nHeadPos;
	size_t			m_nTailPos;
	char*			m_pBuffer;
	bool			m_bSending;
};

inline void IocpSendBuffer::reset()
{
	m_nUsedSize = 0;
	m_nHeadPos = m_nTailPos = 0;
	m_bSending = false;
}

#endif // __IOCP_SEND_BUFFER_H__

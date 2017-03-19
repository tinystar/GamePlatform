/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/2
/************************************************************************/
#ifndef __CLIENT_CONTEXT_H__
#define __CLIENT_CONTEXT_H__

#include <WinSock2.h>
#include "xEzUtil.h"
#include "IoContext.h"
#include "IClientSessionMgr.h"
#include "IocpSendBuffer.h"
#include "IocpRecvBuffer.h"


class ClientContext
{
EZ_FORBID_CLASS_COPY(ClientContext)

public:
	enum Status
	{
		kNewCreated = 0,
		kInitialized,
		kConnected,
		kClosing,
		kClosed,
	};

public:
	ClientContext()
		: m_hSocket(INVALID_SOCKET)
		, m_pClientMgr(NULL)
		, m_nMaxPkgSize(0)
		, m_status(kNewCreated)
		, m_pUserData(NULL)
		, m_bWaitSend(false)
		, m_nWaitSendSize(0)
	{
		::memset(&m_address, 0, sizeof(m_address));
		m_hWaitSendEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		EzAssert(m_hWaitSendEvent != NULL);
	}
	~ClientContext()
	{
		if (m_hSocket != INVALID_SOCKET)
		{
			::closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
		}

		if (m_hWaitSendEvent != NULL)
			::CloseHandle(m_hWaitSendEvent);
	}

	bool init(IClientSessionMgr* pManager, size_t nMaxPkgSize);

	bool attach(SOCKET hSocket, const SOCKADDR_IN& addr);

	SOCKET getSocket() const { return m_hSocket; }
	const SOCKADDR_IN& getAddress() const { return m_address; }

	bool asyncRecv();
	bool asyncSend(void* pData, size_t nDataLen);

	bool onRecvCompleted();
	bool onSendCompleted(size_t nSize);

	bool closeClient(bool bGraceful);

	Status getStatus() const { return m_status; }
	bool isValid() const { return m_status == kConnected; }

	void setUserData(void* pData) { m_pUserData = pData; }
	void* getUserData() const { return m_pUserData; }

	void dump(EzDumpContext& dc) const;
	const char* Status2String(Status status) const;

private:
	IClientSessionMgr*	m_pClientMgr;
	Status				m_status;
	SOCKET				m_hSocket;
	SOCKADDR_IN			m_address;			// client address
	size_t				m_nMaxPkgSize;
	PER_IO_CONTEXT		m_sendIoContext;
	PER_IO_CONTEXT		m_recvIoContext;
	IocpSendBuffer		m_sendBuffer;
	IocpRecvBuffer		m_recvBuffer;
	EzLock				m_sendLock;
	EzLock				m_recvLock;
	EzLock				m_closeLock;
	HANDLE				m_hWaitSendEvent;
	bool				m_bWaitSend;
	size_t				m_nWaitSendSize;
	void*				m_pUserData;
};

inline bool ClientContext::attach(SOCKET hSocket, const SOCKADDR_IN& addr)
{
	if (!EzVerify(kInitialized == m_status || kClosed == m_status))
		return false;

	if (!EzVerify(hSocket != INVALID_SOCKET))
		return false;

	EzAssert(m_hSocket == INVALID_SOCKET);
	m_hSocket = hSocket;
	m_address = addr;

	m_sendIoContext.type = eOpSend;
	m_recvIoContext.type = eOpRecv;

	m_status = kConnected;

	return true;
}

#endif // __CLIENT_CONTEXT_H__

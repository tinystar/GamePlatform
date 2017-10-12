/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/17
/************************************************************************/
#ifndef __TCP_CLIENTSOCKET_IMP_H__
#define __TCP_CLIENTSOCKET_IMP_H__

#include "TcpClientSocket.h"
#include <map>


class TcpClientSocketImp
{
public:
	TcpClientSocketImp(TcpClientSocket* pApiObj, size_t nMaxPkgSize);
	~TcpClientSocketImp();

public:
	static void select(unsigned int nTimeOut);

public:
	SOCKET getSocket() const { return m_hSocket; }
	TcpClientSocket::Status getStatus() const { return m_sockStatus; }

	int create(bool bBlocking);
	int connect(const char* pszAddress, unsigned short uPort);
	int sendData(void* pData, size_t nDataLen);
	void close(int nErrCode = SEC_SUCCESS);

public:
	bool addEventHandler(ITcpClientSocketEventHandler* pEventHandler);
	bool removeEventHandler(ITcpClientSocketEventHandler* pEventHandler);

protected:
	bool initBuffer(size_t nPkgSize);
	void clearBuffer();
	bool copyToSendBuf(void* pData, size_t nDataLen);
	int sendBlocking(void* pData, size_t nDataLen);
	int doSend();
	int doRecv();

	void notifyConnectedEvent();
	void notifyRecvedEvent(void* pPackage, size_t nSize);
	void notifyClosedEvent(int nErrCode);

	int translateErrCode(int nSysCode);

protected:
	typedef EzArray<ITcpClientSocketEventHandler*>	EventHandlerArray;
	typedef std::map<SOCKET, TcpClientSocketImp*>	SocketToClientMap;

	static SocketToClientMap	ms_SocketsMap;
	static EzLock				ms_mapLock;

	TcpClientSocket*		m_pApiObj;
	size_t					m_nMaxPkgSize;
	SOCKET					m_hSocket;
	bool					m_bBlocking;
	EventHandlerArray		m_EventHandlers;
	char*					m_pSendBuffer;
	size_t					m_nSendBufSize;
	size_t					m_nSendBufUsed;
	char*					m_pRecvBuffer;
	size_t					m_nRecvBufSize;
	size_t					m_nRecvBufUsed;
	EzLock					m_sendLock;
	TcpClientSocket::Status	m_sockStatus;
	EzLock					m_statusLock;
	bool					m_bWaitSend;
};

#endif // __TCP_CLIENTSOCKET_IMP_H__
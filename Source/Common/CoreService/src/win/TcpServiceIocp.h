/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/11/28
/************************************************************************/
#ifndef __TCP_SERVICE_IOCP_H__
#define __TCP_SERVICE_IOCP_H__

#include "TcpService.h"
#include "ClientContextMgr.h"
#include "IClientSessionMgr.h"
#include <vector>
#include <list>

struct PER_ACCEPT_CONTEXT;


/**
 * IOCP µœ÷
 */
class TcpServiceIocp : public TcpService, public IClientSessionMgr
{
public:
	TcpServiceIocp();
	virtual ~TcpServiceIocp();

	// TcpService interfaces
public:
	virtual SVCErrorCode init(const TcpConfig& config = TcpConfig::kDefault);
	virtual SVCErrorCode unInit();
	virtual SVCErrorCode start();
	virtual SVCErrorCode stop();

	virtual bool sendData(ClientId id, void* pData, size_t nDataLen);
	virtual bool sendDataToAll(void* pData, size_t nDataLen);

	virtual bool closeClient(ClientId id, bool bGraceful = true);

	virtual size_t getClientCount() const;

	virtual ClientIdIterator* newIterator() const;

	virtual void dump(EzDumpContext& dc) const;

	virtual void finalReleaseClient(ClientId id);

	// IClientSessionMgr interfaces
public:
	virtual bool OnErrorOccurredNotify(ClientContext* pClient, SVCErrorCode errType, int iErrCode);
	virtual bool OnPackageRecvedNotify(ClientContext* pClient, void* pPackage, size_t nSize);

protected:
	SVCErrorCode initFromConfig(const TcpConfig& config);

	SVCErrorCode initListenSock();
	SVCErrorCode initIocp();

	bool postAccept(PER_ACCEPT_CONTEXT* pAcceptContext);
	bool doAccept(PER_ACCEPT_CONTEXT* pAcceptContext, DWORD dwBytes);

	bool closeAndFreeClient(ClientContext* pClient, bool bGraceful);
	void closeAllClient();

	void notifyCloseClient(ClientContext* pClient, bool bGraceful);

	bool initSendToAllThread();
	void stopSendToAllThread();
	void sendToAllProc();

	CSHANDLE generateUniqueHandle();

	static unsigned __stdcall iocpWorkerThread(void* pParam);
	static unsigned __stdcall sendToAllThread(void* pParam);

private:
	struct SendDataCache
	{
		void*	pData;
		size_t	nDataLen;
	};

	typedef std::vector<PER_ACCEPT_CONTEXT*> AcceptContextArray;
	typedef std::list<SendDataCache>		 SendCacheList;

	ULONG								m_ulAddress;
	USHORT								m_usPort;
	SOCKET								m_listenSock;
	HANDLE								m_hIocp;
	unsigned int						m_nThreadCount;
	HANDLE								m_hThreadEvent;
	unsigned int						m_nAcceptCount;
	size_t								m_nPackageSize;
	bool								m_bInitialized;
	bool								m_bRunning;
	AcceptContextArray					m_AcceptContexts;
	ClientContextMgr					m_ClientContextMgr;
	HANDLE								m_hSendToAllThread;
	HANDLE								m_hSendCacheSem;
	HANDLE								m_hSendQuitEvent;
	SendCacheList						m_sendDataCache;
	EzLock								m_cacheListLock;

	CSHANDLE							m_handleSeed;
};

#endif // __TCP_SERVICE_IOCP_H__
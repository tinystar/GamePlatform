/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/3/23
/************************************************************************/
#ifndef __SERVER_TEMPLATE_IMP_H__
#define __SERVER_TEMPLATE_IMP_H__

#include "ServerTemplate.h"
#include <list>
#include "QueueItemDef.h"


class ServerTemplateImp : public ITcpServiceEventHandler, public ITimerServiceEventHandler
{
	friend class ServerTemplate;

protected:
	ServerTemplateImp(ServerTemplate* pApiObj);
	virtual ~ServerTemplateImp();
	
	bool start();
	bool stop();

protected:
	virtual void onClientConnected(ClientId id);
	virtual void onPackageReceived(ClientId id, void* pPackage, size_t nSize);
	virtual void onClientClosed(ClientId id);

	virtual void onTimerMessage(EzUInt uTimerId);

protected:
	static unsigned __stdcall msgQueueHandleThread(void* pParam);

	bool addQueueItem(QueueItemHead* pItem);

	void handleQueueMsg();

	bool queueUserItem(int itemId, void* pData, size_t nSize);

	bool isQueueEmpty() const;

protected:
	ServerTemplate*				m_pApiObj;

	std::list<QueueItemHead*>	m_msgQueue;
	HANDLE						m_hSemaphore;
	HANDLE						m_hQuitEvent;
	HANDLE						m_hMsgThread;
	EzLock						m_queueLock;
	bool						m_bRunning;
};

#endif // __SERVER_TEMPLATE_IMP_H__
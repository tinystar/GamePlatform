/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/8
/************************************************************************/
#ifndef __CLIENT_CONTEXT_MGR_H__
#define __CLIENT_CONTEXT_MGR_H__

#include "xEzUtil.h"
#include "ClientContext.h"
#include "ClientIdIterator.h"


class ClientContextMgr
{
	friend class ClientCtxIterator;

public:
	ClientContextMgr();
	~ClientContextMgr();

	ClientContext* newClientContext();
	void freeClientContext(ClientContext* pClient);

	size_t getClientCount() const { return m_ActiveClientList.size(); }
	size_t getFreeCount() const { return m_FreeClientList.size(); }

	ClientCtxIterator* newIterator() const;

	void clearFreeClientList();

private:
	typedef EzDoublyLinkedList<ClientContext>	ClientContextList;
	typedef ClientContextList::Node				ClientContextNode;
	typedef ClientContextList::Iterator			ClientContextIterator;

	void attachIterator(ClientCtxIterator* pIter);
	void detachIterator(ClientCtxIterator* pIter);
	void notifyIteratorsBeforeRemove(ClientContextNode* pClientNode);
	void notifyIteratorsAfterRemove();

private:
	ClientContextList			m_ActiveClientList;
	ClientContextList			m_FreeClientList;
	EzLock						m_ActiveListLock;
	EzLock						m_FreeListLock;
	EzArray<ClientCtxIterator*>	m_iters;
	EzLock						m_itersLock;
};


class ClientCtxIterator : public ClientIdIterator
{
	friend class ClientContextMgr;

public:
	virtual ~ClientCtxIterator();

	virtual bool start();
	virtual bool step();
	virtual bool isDone();

	virtual ClientId getClientId() { return ClientId(getClientCtx()); }

	ClientContext* getClientCtx();

private:
	typedef ClientContextMgr::ClientContextList	ClientCtxList;
	typedef ClientContextMgr::ClientContextNode	ClientCtxNode;

	ClientCtxIterator(ClientContextMgr* pClientCtxMgr, const ClientCtxList* pList);

	void beforeClientRemoveNotify(ClientCtxNode* pClient);
	void afterClientRemoveNotify();

private:
	ClientContextMgr*		m_pClientCtxMgr;
	const ClientCtxList*	m_pClientList;
	ClientCtxList::Iterator	m_curIter;
	EzLock					m_nodeLock;
	HANDLE					m_hRemoveWaitEvent;
	ClientCtxNode*			m_pNodeToBeRemove;
	bool					m_bNeedWait;
};

#endif // __CLIENT_CONTEXT_MGR_H__

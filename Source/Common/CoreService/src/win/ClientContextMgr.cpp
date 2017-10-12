#include "ClientContextMgr.h"


//-------------------------------------------------------------------------------
// ClientContextMgr
//-------------------------------------------------------------------------------
ClientContextMgr::ClientContextMgr()
{

}

ClientContextMgr::~ClientContextMgr()
{
	releaseClosedArray();
}

ClientContext* ClientContextMgr::newClientContext()
{
	ClientContextNode* pClientNode = NULL;

	m_FreeListLock.lock();
	if (m_FreeClientList.isEmpty())
	{
		m_FreeListLock.unlock();
		pClientNode = new ClientContextNode;
	}
	else
	{
		pClientNode = m_FreeClientList.pop_front();
		m_FreeListLock.unlock();
	}

	if (!EzVerify(pClientNode))
		return NULL;

	m_ActiveListLock.lock();
	m_ActiveClientList.push_back(pClientNode);
	m_ActiveListLock.unlock();

	return &pClientNode->value;
}

void ClientContextMgr::closeClientContext(ClientContext* pClient)
{
	if (NULL == pClient)
		return;

	ClientContextNode* pClientNode = CONTAINING_RECORD(pClient, ClientContextNode, value);
	EzAssert(pClientNode != NULL);

	m_ActiveListLock.lock();
	EzAssert(m_ActiveClientList.size() > 0);
	notifyIteratorsBeforeRemove(pClientNode);			// notify iterators before remove node form the list.
	m_ActiveClientList.remove(pClientNode);
	notifyIteratorsAfterRemove();						// notify iterators after remove node from the list.
	m_ActiveListLock.unlock();

	m_ClosedArrLock.lock();
	m_ClosedArray.append(pClient);
	m_ClosedArrLock.unlock();
}

void ClientContextMgr::freeClientContext(ClientContext* pClient)
{
	if (NULL == pClient)
		return;

	m_ClosedArrLock.lock();
	int idx = m_ClosedArray.find(pClient);
	if (idx < 0)
	{
		m_ClosedArrLock.unlock();
		return;
	}

	m_ClosedArray.removeAt(idx);
	m_ClosedArrLock.unlock();

	ClientContextNode* pClientNode = CONTAINING_RECORD(pClient, ClientContextNode, value);
	EzAssert(pClientNode != NULL);

	m_FreeListLock.lock();
	m_FreeClientList.push_back(pClientNode);
	m_FreeListLock.unlock();
}

void ClientContextMgr::closeAndFreeClientContext(ClientContext* pClient)
{
	closeClientContext(pClient);
	freeClientContext(pClient);
}

ClientCtxIterator* ClientContextMgr::newIterator() const
{
	return new ClientCtxIterator(const_cast<ClientContextMgr*>(this), &m_ActiveClientList);
}

void ClientContextMgr::clearFreeClientList()
{
	m_FreeListLock.lock();
	m_FreeClientList.clear();
	m_FreeListLock.unlock();
}

void ClientContextMgr::releaseClosedArray()
{
	m_ClosedArrLock.lock();
	for (int i = 0; i < m_ClosedArray.logicalLength(); ++i)
	{
		ClientContextNode* pClientNode = CONTAINING_RECORD(m_ClosedArray[i], ClientContextNode, value);
		delete pClientNode;
	}
	m_ClosedArray.removeAll();
	m_ClosedArrLock.unlock();
}

void ClientContextMgr::attachIterator(ClientCtxIterator* pIter)
{
	EzAutoLocker autoLocker(&m_itersLock);
	if (pIter != NULL)
		m_iters.append(pIter);
}

void ClientContextMgr::detachIterator(ClientCtxIterator* pIter)
{
	EzAutoLocker autoLocker(&m_itersLock);
	m_iters.remove(pIter);
}

void ClientContextMgr::notifyIteratorsBeforeRemove(ClientContextNode* pClientNode)
{
	EzAutoLocker autoLocker(&m_itersLock);
	for (int i = 0; i < m_iters.logicalLength(); ++i)
	{
		ClientCtxIterator* pIter = m_iters[i];
		pIter->beforeClientRemoveNotify(pClientNode);
	}
}

void ClientContextMgr::notifyIteratorsAfterRemove()
{
	EzAutoLocker autoLocker(&m_itersLock);
	for (int i = 0; i < m_iters.logicalLength(); ++i)
	{
		ClientCtxIterator* pIter = m_iters[i];
		pIter->afterClientRemoveNotify();
	}
}


//-------------------------------------------------------------------------------
// ClientCtxIterator
//-------------------------------------------------------------------------------
ClientCtxIterator::ClientCtxIterator(ClientContextMgr* pClientCtxMgr, const ClientCtxList* pList)
	: m_pClientCtxMgr(pClientCtxMgr)
	, m_pClientList(pList)
	, m_hRemoveWaitEvent(INVALID_HANDLE_VALUE)
	, m_pNodeToBeRemove(NULL)
	, m_bNeedWait(false)
{
	EzAssert(m_pClientCtxMgr != NULL);
	if (m_pClientCtxMgr != NULL)
		m_pClientCtxMgr->attachIterator(this);

	m_hRemoveWaitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	EzAssert(m_hRemoveWaitEvent != NULL);

	start();
}

ClientCtxIterator::~ClientCtxIterator()
{
	if (m_pClientCtxMgr)
		m_pClientCtxMgr->detachIterator(this);

	::CloseHandle(m_hRemoveWaitEvent);
}

bool ClientCtxIterator::start()
{
	if (NULL == m_pClientList)
		return false;

	m_nodeLock.lock();
	m_curIter = m_pClientList->head();
	m_nodeLock.unlock();
	return true;
}

bool ClientCtxIterator::step()
{
	if (NULL == m_pClientList)
		return false;

	m_nodeLock.lock();
	++m_curIter;
	if (m_pNodeToBeRemove != NULL && m_pNodeToBeRemove == m_curIter.current())
		++m_curIter;
	if (m_bNeedWait)
	{
		::SetEvent(m_hRemoveWaitEvent);
		m_bNeedWait = false;			// SetEvent后应该立即将m_bNeedWait置为false，否则如果再次进入step，还会SetEvent导致同步失败
	}
	m_nodeLock.unlock();
	return true;
}

bool ClientCtxIterator::isDone()
{
	if (NULL == m_pClientList)
		return true;

	return m_curIter == m_pClientList->end();
}

ClientContext* ClientCtxIterator::getClientCtx()
{
	if (NULL == m_pClientList)
		return NULL;

	return &m_curIter->value;
}

void ClientCtxIterator::beforeClientRemoveNotify(ClientCtxNode* pClient)
{
	m_nodeLock.lock();
	m_pNodeToBeRemove = pClient;
	if (pClient == m_curIter.current())
		m_bNeedWait = true;
	m_nodeLock.unlock();

	if (m_bNeedWait)
		::WaitForSingleObject(m_hRemoveWaitEvent, INFINITE);

	m_nodeLock.lock();
}

void ClientCtxIterator::afterClientRemoveNotify()
{
	m_pNodeToBeRemove = NULL;
	m_nodeLock.unlock();
}

#include "ClientId.h"
#ifdef EZ_WINDOWS
#include "win/ClientContext.h"
#endif


const ClientId ClientId::kNull;

bool ClientId::isValid() const
{
	if (NULL == m_pClientCtx)
		return false;

	return m_pClientCtx->isValid();
}

SOCKADDR_IN ClientId::getAddress() const
{
	if (NULL == m_pClientCtx)
	{
		SOCKADDR_IN dummyAddr;
		::memset(&dummyAddr, 0, sizeof(dummyAddr));
		return dummyAddr;
	}

	return m_pClientCtx->getAddress();
}

bool ClientId::operator<(const ClientId& rhs) const
{
	return m_pClientCtx < rhs.m_pClientCtx;
}

bool ClientId::operator>(const ClientId& rhs) const
{
	return m_pClientCtx > rhs.m_pClientCtx;
}

bool ClientId::operator==(const ClientId& rhs) const
{
	return m_pClientCtx == rhs.m_pClientCtx;
}

bool ClientId::operator!=(const ClientId& rhs) const
{
	return m_pClientCtx != rhs.m_pClientCtx;
}

void ClientId::setUserData(void* pData)
{
	if (m_pClientCtx != NULL)
		m_pClientCtx->setUserData(pData);
}

void* ClientId::getUserData() const
{
	return (m_pClientCtx != NULL) ? m_pClientCtx->getUserData() : NULL;
}

void ClientId::dump(EzDumpContext& dc) const
{
	if (NULL == m_pClientCtx)
	{
		dc << "NULL ClientId\n";
		return;
	}

	m_pClientCtx->dump(dc);
}

#include "MainServerList.h"


MainServerList::MainServerList()
{

}

MainServerList::~MainServerList()
{

}

void MainServerList::addServer(const MainSvrNode& svr)
{
	m_svrList.append(svr);
}

void MainServerList::removeServer(const ClientId& id)
{
	for (int i = 0; i < m_svrList.logicalLength(); ++i)
	{
		if (m_svrList[i].svrId == id)
		{
			m_svrList.swap(i, m_svrList.logicalLength() - 1);
			m_svrList.removeLast();
			break;
		}
	}
}

void MainServerList::addServerUser(const ClientId& id)
{
	for (int i = 0; i < m_svrList.logicalLength(); ++i)
	{
		if (m_svrList[i].svrId == id)
		{
			m_svrList[i].uOnlineUser++;
			break;
		}
	}
}

void MainServerList::removeServerUser(const ClientId& id)
{
	for (int i = 0; i < m_svrList.logicalLength(); ++i)
	{
		if (m_svrList[i].svrId == id)
		{
			m_svrList[i].uOnlineUser--;
			break;
		}
	}
}

const MainSvrNode* MainServerList::chooseServer() const
{
	unsigned int uMin = MAXUINT;
	int idxMin = -1;
	for (int i = 0; i < m_svrList.logicalLength(); ++i)
	{
		unsigned int uOnlineCnt = m_svrList[i].uOnlineUser;
		unsigned int uThreshold = (unsigned int)(0.8 * m_svrList[i].uMaxUser);		// 80%
		if (uOnlineCnt < uThreshold)
			return &m_svrList[i];

		if (uOnlineCnt < m_svrList[i].uMaxUser && uOnlineCnt < uMin)
		{
			uMin = uOnlineCnt;
			idxMin = i;
		}
	}

	if (idxMin >= 0)
		return &m_svrList[idxMin];

	return NULL;
}

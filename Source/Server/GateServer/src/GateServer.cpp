#include "GateServer.h"
#include "GateMsgDefs.h"

#define CLIENT_TYPE_MAIN		1


BEGIN_NETMSG_TABLE(GateServer, BaseGameServer)
	ON_NET_MESSAGE(MSG_MAINID_GATE, MSG_SUBID_REQUEST_CONFIG, &GateServer::onRequestConfig)
	ON_NET_MESSAGE(MSG_MAINID_GATE, MSG_SUBID_REQUEST_MAINSVR_ADDR, &GateServer::onRequestMainAddr)
	ON_NET_MESSAGE(MSG_MAINID_MAIN_TO_GATE, MSG_SUBID_MAIN_CONNECT, &GateServer::onMainConnect)
END_NETMSG_TABLE()


GateServer::GateServer()
	: m_pUIObserver(NULL)
{
	::memset(m_szVersion, 0, sizeof(m_szVersion));
	::memset(m_szUpdUrl, 0, sizeof(m_szUpdUrl));
}

GateServer::~GateServer()
{

}

void GateServer::onRequestConfig(ClientId id, void* pData, size_t nDataLen)
{
	GateConfigMsg msg;
	msg.header.uMainId = MSG_MAINID_GATE;
	msg.header.uSubId = MSG_SUBID_CONFIG;
	strcpy(msg.szVersion, m_szVersion);
	strcpy(msg.szUpdUrl, m_szUpdUrl);

	sendMsg(id, &msg, sizeof(msg));
}

void GateServer::onRequestMainAddr(ClientId id, void* pData, size_t nDataLen)
{
	const MainSvrNode* pSvrNode = m_mainSvrList.chooseServer();
	if (NULL == pSvrNode)
	{
		sendMsg(id, MSG_MAINID_GATE, MSG_SUBID_NO_MAINSVR);
		return;
	}

	MainAddressMsg msg;
	msg.header.uMainId = MSG_MAINID_GATE;
	msg.header.uSubId = MSG_SUBID_MAINSVR_ADDR;
	memcpy(msg.szIP, pSvrNode->szMainAddr, sizeof(msg.szIP));
	msg.sPort = pSvrNode->sMainPort;
	
	sendMsg(id, &msg, sizeof(msg));
}

void GateServer::onMainConnect(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(MainConnectMsg) == nDataLen);

	MainConnectMsg* pConnMsg = (MainConnectMsg*)pData;
	
	MainSvrNode svrNode;
	svrNode.svrId = id;
	::strcpy(svrNode.szMainAddr, inet_ntoa(id.getAddress().sin_addr));
	svrNode.sMainPort = pConnMsg->sPort;
	svrNode.uMaxUser = pConnMsg->uMaxUser;
	m_mainSvrList.addServer(svrNode);

	id.setUserData((void*)CLIENT_TYPE_MAIN);

	if (m_pUIObserver != NULL)
		m_pUIObserver->onUIMainServerAdded(id, svrNode);
}

void GateServer::onTcpClientCloseMsg(ClientId id)
{
	int nType = (int)(id.getUserData());
	if (CLIENT_TYPE_MAIN == nType)
	{
		m_mainSvrList.removeServer(id);

		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIMainServerRemoved(id);
	}
}

bool GateServer::setVersion(const char* pszVer)
{
	if (NULL == pszVer || strlen(pszVer) > (sizeof(m_szVersion) - 1))
		return false;

	strcpy(m_szVersion, pszVer);
	return true;
}

bool GateServer::setUpdUrl(const char* pszUrl)
{
	if (NULL == pszUrl || strlen(pszUrl) > (sizeof(m_szUpdUrl) - 1))
		return false;

	strcpy(m_szUpdUrl, pszUrl);
	return true;
}

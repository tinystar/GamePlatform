#include "GateServer.h"
#include "GateMsgDefs.h"

#define CLIENT_TYPE_MAIN		1


NetMsgMapEntry GateServer::s_msgMapArray[] = {
	{ MSG_MAINID_GATE, MSG_SUBID_REQUEST_CONFIG, static_cast<NetMsgHandler>(&GateServer::onRequestConfig) },
	{ MSG_MAINID_GATE, MSG_SUBID_REQUEST_MAINSVR_ADDR, static_cast<NetMsgHandler>(&GateServer::onRequestMainAddr) },
	{ MSG_MAINID_MAIN_TO_GATE, MSG_SUBID_MAIN_CONNECT, static_cast<NetMsgHandler>(&GateServer::onMainConnect) }
};


GateServer::GateServer()
	: m_pUIObserver(NULL)
{
	::memset(m_szVersion, 0, sizeof(m_szVersion));
	::memset(m_szUpdUrl, 0, sizeof(m_szUpdUrl));
}

GateServer::~GateServer()
{

}

bool GateServer::onInit(const ServerInitConfig&)
{
	registerMsgHandler(s_msgMapArray, EzCountOf(s_msgMapArray));
	return true;
}

bool GateServer::onUninit()
{
	removeMsgHandler(s_msgMapArray, EzCountOf(s_msgMapArray));
	return true;
}

bool GateServer::onStart()
{
	return true;
}

bool GateServer::onStop()
{
	return true;
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
	if (strlen(pszVer) > 15)
		return false;

	strcpy(m_szVersion, pszVer);
	return true;
}

bool GateServer::setUpdUrl(const char* pszUrl)
{
	if (strlen(pszUrl) > 255)
		return false;

	strcpy(m_szUpdUrl, pszUrl);
	return true;
}

#include "MainServer.h"
#include "MainMsgDefs.h"
#include <process.h>
#include "GateMsgDefs.h"

#define ITEM_ID_GATE		1
#define ITEM_ID_DB			2


MainServer::MainServer()
	: m_sGatePort(0)
	, m_sDBPort(0)
	, m_bStopServer(false)
	, m_hSelectThread(NULL)
	, m_sPort(0)
	, m_uMaxUser(0)
	, m_pUIObserver(NULL)
{
	::memset(m_szGateAddr, 0, sizeof(m_szGateAddr));
	::memset(m_szDBAddr, 0, sizeof(m_szDBAddr));

	m_clientToGate.addEventHandler(this);
	m_clientToDB.addEventHandler(this);
}

MainServer::~MainServer()
{
	m_clientToDB.removeEventHandler(this);
	m_clientToGate.removeEventHandler(this);
}

bool MainServer::onInit(const ServerInitConfig& serverConfig)
{
	m_sPort = serverConfig.tcpConfig.sPort;
	return true;
}

bool MainServer::onUninit()
{
	return true;
}

bool MainServer::onStart()
{
	if (!connectToGate())
		return false;
	if (!connectToDB())
		return false;

	m_bStopServer = false;
	m_hSelectThread = (HANDLE)::_beginthreadex(NULL, 0, clientSelectThread, this, 0, NULL);
	if (NULL == m_hSelectThread)
		return false;

	return true;
}

bool MainServer::onStop()
{
	m_bStopServer = true;
	::WaitForSingleObject(m_hSelectThread, INFINITE);
	::CloseHandle(m_hSelectThread);
	m_hSelectThread = NULL;

	m_clientToGate.close();
	m_clientToDB.close();
	return true;
}

void MainServer::setGateSvrAddr(const char* pszAddr, unsigned short sPort)
{
	strncpy(m_szGateAddr, pszAddr, 20);
	m_sGatePort = sPort;
}

void MainServer::setDBSvrAddr(const char* pszAddr, unsigned short sPort)
{
	strncpy(m_szDBAddr, pszAddr, 20);
	m_sDBPort = sPort;
}

void MainServer::onSocketConnected(TcpClientSocket* pClientSock)
{
	if (pClientSock == &m_clientToGate)
	{
		MainConnectMsg connMsg;
		connMsg.header.uMainId = MSG_MAINID_MAIN_TO_GATE;
		connMsg.header.uSubId = MSG_SUBID_MAIN_CONNECT;
		connMsg.sPort = m_sPort;
		connMsg.uMaxUser = m_uMaxUser;

		sendMsgToServer(pClientSock, &connMsg, sizeof(connMsg));

		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToGateSuccess();
	}
	else if (pClientSock == &m_clientToDB)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToDBSuccess();
	}
}

void MainServer::onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize)
{
	if (pClientSock == &m_clientToGate)
		queueUserItem(ITEM_ID_GATE, pPackage, nSize);
	else if (pClientSock == &m_clientToDB)
		queueUserItem(ITEM_ID_DB, pPackage, nSize);
}

void MainServer::onSocketClosed(TcpClientSocket* pClientSock, int nErrCode)
{
	if (pClientSock == &m_clientToGate)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToGateClosed();

		if (!m_bStopServer && nErrCode != SEC_SUCCESS && nErrCode != SEC_CLOSEDBYPEER && nErrCode != SEC_CONNRESET)
			connectToGate();
	}
	else if (pClientSock == &m_clientToDB)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToDBClosed();

		if (!m_bStopServer && nErrCode != SEC_SUCCESS && nErrCode != SEC_CLOSEDBYPEER && nErrCode != SEC_CONNRESET)
			connectToDB();
	}
}

void MainServer::onTimerMsg(EzUInt uTimerId)
{
	BaseGameServer::onTimerMsg(uTimerId);
}

void MainServer::onUserItemMsg(int itemId, void* pData, size_t nSize)
{
	if (ITEM_ID_GATE == itemId)
		onGateServerMsg(pData, nSize);
	else if (ITEM_ID_DB == itemId)
		onDBServerMsg(pData, nSize);
}

unsigned __stdcall MainServer::clientSelectThread(void* pParam)
{
	MainServer* pMainSvr = (MainServer*)pParam;

	while (!pMainSvr->m_bStopServer)
	{
		TcpClientSocket::select(5);
	}

	return 0;
}

bool MainServer::connectToGate()
{
	if (m_clientToGate.create() != SEC_SUCCESS)
		return false;
	int nCode = m_clientToGate.connect(m_szGateAddr, m_sGatePort);
	if (nCode != SEC_SUCCESS)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToGateFail(nCode);
		return false;
	}
	return true;
}

bool MainServer::connectToDB()
{
	if (m_clientToDB.create() != SEC_SUCCESS)
		return false;
	int nCode = m_clientToDB.connect(m_szDBAddr, m_sDBPort);
	if (nCode != SEC_SUCCESS)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToDBFail(nCode);
		return false;
	}
	return true;
}

void MainServer::onGateServerMsg(void* pData, size_t nSize)
{

}

void MainServer::onDBServerMsg(void* pData, size_t nSize)
{

}

bool MainServer::sendMsgToServer(TcpClientSocket* pClientSock, void* pData, size_t nDataLen)
{
	if (NULL == pClientSock || NULL == pData || 0 == nDataLen)
		return false;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)pData, nDataLen);

	pClientSock->sendData(pData, nDataLen);
	return true;
}

bool MainServer::sendMsgToServer(TcpClientSocket* pClientSock, CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved /*= 0*/)
{
	GameMsgHeader header;
	header.uMainId = uMainId;
	header.uSubId = uSubId;
	header.uReserved = uReserved;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)&header, sizeof(header));

	pClientSock->sendData(&header, sizeof(header));
	return true;
}

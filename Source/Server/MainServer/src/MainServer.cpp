#include "MainServer.h"
#include "MainMsgDefs.h"


MainServer::MainServer()
	: m_sGatePort(0)
	, m_sDBPort(0)
{
	::memset(m_szGateAddr, 0, sizeof(m_szGateAddr));
	::memset(m_szDBAddr, 0, sizeof(m_szDBAddr));
}

MainServer::~MainServer()
{

}

bool MainServer::onInit()
{
	return true;
}

bool MainServer::onUninit()
{
	return true;
}

bool MainServer::onStart()
{
	return true;
}

bool MainServer::onStop()
{
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

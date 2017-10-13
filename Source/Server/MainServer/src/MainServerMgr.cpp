#include "MainServerMgr.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

using namespace rapidjson;

#define MAIN_CONFIG_FILE			_T("MainConfig.json")
#define DEF_MAINSVR_PORT			5050
#define DEF_PACKAGE_SIZE			1024


MainServerMgr::MainServerMgr()
	: m_bDebugMode(false)
{
	m_tcpConfig.sPort = DEF_MAINSVR_PORT;
	m_tcpConfig.nMaxPackageSize = DEF_PACKAGE_SIZE;
}

SVCErrorCode MainServerMgr::initServer()
{
	ServerInitConfig initCfg;
	initCfg.tcpConfig = m_tcpConfig;
	return m_server.init(initCfg);
}

SVCErrorCode MainServerMgr::unInitServer()
{
	return m_server.unInit();
}

SVCErrorCode MainServerMgr::startServer()
{
	return m_server.start();
}

SVCErrorCode MainServerMgr::stopServer()
{
	return m_server.stop();
}

bool MainServerMgr::loadConfig()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	bool bRet = EzSys::getCurModulePath(szPath, MAX_PATH);
	if (!bRet)
		return false;

	size_t nLen = _tcslen(szPath);
	nLen += _tcslen(MAIN_CONFIG_FILE);
	if (nLen >= MAX_PATH)
		return false;

	_tcscat_s(szPath, MAX_PATH, MAIN_CONFIG_FILE);

	FILE* fp = _tfopen(szPath, _T("r"));
	if (NULL == fp)
		return false;

	char readBuf[8192];
	FileReadStream is(fp, readBuf, sizeof(readBuf));
	fclose(fp);

	Document doc;
	doc.ParseStream(is);

	m_bDebugMode = doc["debugMode"].GetBool();
	LogLevel logLevel = (LogLevel)doc["logLevel"].GetInt();
	unsigned int uMaxUser = doc["maxUser"].GetUint();
	const Value& tcpCfg = doc["tcpConfig"];
	m_tcpConfig.sPort = (unsigned short)tcpCfg["port"].GetInt();
	m_tcpConfig.nSockThreadCnt = tcpCfg["threadCount"].GetUint();
	m_tcpConfig.nMaxAcceptCnt = tcpCfg["acceptCount"].GetUint();
	m_tcpConfig.nMaxPackageSize = tcpCfg["pkgSize"].GetUint();
	const Value& gateSvrCfg = doc["GateSvrAddr"];
	const char* pszGateAddr = gateSvrCfg["addr"].GetString();
	unsigned short sGatePort = (unsigned short)gateSvrCfg["port"].GetInt();
	const Value& DBSvrCfg = doc["DBSvrAddr"];
	const char* pszDBAddr = DBSvrCfg["addr"].GetString();
	unsigned short sDBPort = (unsigned short)DBSvrCfg["port"].GetInt();

	EzLogger::setLogLevel(logLevel);

	m_server.setMaxUser(uMaxUser);
	m_server.setGateSvrAddr(pszGateAddr, sGatePort);
	m_server.setDBSvrAddr(pszDBAddr, sDBPort);

	return true;
}

void MainServerMgr::setGateSvrAddr(const char* pszAddr, unsigned short sPort)
{
	m_server.setGateSvrAddr(pszAddr, sPort);
}

bool MainServerMgr::getGateSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort) const
{
	const char* pszIP = m_server.getGateSvrIP();
	if (strlen(pszIP) >= nSize)
		return false;

	strncpy(pszAddr, pszIP, strlen(pszIP));
	pszAddr[strlen(pszIP)] = 0;
	sPort = m_server.getGateSvrPort();
	return true;
}

void MainServerMgr::setDBSvrAddr(const char* pszAddr, unsigned short sPort)
{
	m_server.setDBSvrAddr(pszAddr, sPort);
}

bool MainServerMgr::getDBSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort) const
{
	const char* pszIP = m_server.getDBSvrIP();
	if (strlen(pszIP) >= nSize)
		return false;

	strncpy(pszAddr, pszIP, strlen(pszIP));
	sPort = m_server.getDBSvrPort();
	return true;
}

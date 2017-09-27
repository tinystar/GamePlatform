#include "MainServerMgr.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

using namespace rapidjson;

#define MAIN_CONFIG_FILE			_T("MainConfig.json")
#define DEF_MAINSVR_PORT			5050
#define DEF_PACKAGE_SIZE			1024


MainServerMgr::MainServerMgr()
	: m_bDebugMode(false)
	, m_logLevel(kLogAll)
	, m_sGatePort(0)
	, m_sDBPort(0)
{
	m_tcpConfig.sPort = DEF_MAINSVR_PORT;
	m_tcpConfig.nMaxPackageSize = DEF_PACKAGE_SIZE;
	::memset(m_szGateAddr, 0, sizeof(m_szGateAddr));
	::memset(m_szDBAddr, 0, sizeof(m_szDBAddr));
}

SVCErrorCode MainServerMgr::initServer()
{
	EzLogger::setLogLevel(m_logLevel);

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
	m_logLevel = (LogLevel)doc["logLevel"].GetInt();
	const Value& tcpCfg = doc["tcpConfig"];
	m_tcpConfig.sPort = (unsigned short)tcpCfg["port"].GetInt();
	m_tcpConfig.nSockThreadCnt = tcpCfg["threadCount"].GetUint();
	m_tcpConfig.nMaxAcceptCnt = tcpCfg["acceptCount"].GetUint();
	m_tcpConfig.nMaxPackageSize = tcpCfg["pkgSize"].GetUint();
	const Value& gateSvrCfg = doc["GateSvrAddr"];
	strncpy(m_szGateAddr, gateSvrCfg["addr"].GetString(), 20);
	m_sGatePort = (unsigned short)gateSvrCfg["port"].GetInt();
	const Value& DBSvrCfg = doc["DBSvrAddr"];
	strncpy(m_szDBAddr, DBSvrCfg["addr"].GetString(), 20);
	m_sDBPort = (unsigned short)DBSvrCfg["port"].GetInt();

	return true;
}

void MainServerMgr::setGateSvrAddr(const char* pszAddr, unsigned short sPort)
{
	strncpy(m_szGateAddr, pszAddr, 20);
	m_sGatePort = sPort;
	m_server.setGateSvrAddr(pszAddr, sPort);
}

bool MainServerMgr::getGateSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort)
{
	if (strlen(m_szGateAddr) >= nSize)
		return false;

	strncpy(pszAddr, m_szGateAddr, strlen(m_szGateAddr));
	pszAddr[strlen(m_szGateAddr)] = 0;
	sPort = m_sGatePort;
	return true;
}

void MainServerMgr::setDBSvrAddr(const char* pszAddr, unsigned short sPort)
{
	strncpy(m_szDBAddr, pszAddr, 20);
	m_sDBPort = sPort;
	m_server.setDBSvrAddr(pszAddr, sPort);
}

bool MainServerMgr::getDBSvrAddr(char* pszAddr, size_t nSize, unsigned short& sPort)
{
	if (strlen(m_szDBAddr) >= nSize)
		return false;

	strncpy(pszAddr, m_szDBAddr, strlen(m_szDBAddr));
	sPort = m_sDBPort;
	return true;
}

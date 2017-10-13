#include "GateServerMgr.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

using namespace rapidjson;

#define GATE_CONFIG_FILE			_T("GateConfig.json")
#define DEF_GATESVR_PORT			5010
#define DEF_PACKAGE_SIZE			1024


GateServerMgr::GateServerMgr()
	: m_bDebugMode(false)
{
	m_tcpConfig.sPort = DEF_GATESVR_PORT;
	m_tcpConfig.nMaxPackageSize = DEF_PACKAGE_SIZE;
}

SVCErrorCode GateServerMgr::initServer()
{
	if (!EzVerify(loadConfig()))
		return eNotApplicable;

	ServerInitConfig initCfg;
	initCfg.tcpConfig = m_tcpConfig;
	return m_server.init(initCfg);
}

SVCErrorCode GateServerMgr::unInitServer()
{
	return m_server.unInit();
}

SVCErrorCode GateServerMgr::startServer()
{
	return m_server.start();
}

SVCErrorCode GateServerMgr::stopServer()
{
	return m_server.stop();
}

bool GateServerMgr::loadConfig()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	bool bRet = EzSys::getCurModulePath(szPath, MAX_PATH);
	if (!bRet)
		return false;

	size_t nLen = _tcslen(szPath);
	nLen += _tcslen(GATE_CONFIG_FILE);
	if (nLen >= MAX_PATH)
		return false;

	_tcscat_s(szPath, MAX_PATH, GATE_CONFIG_FILE);

	FILE* fp = _tfopen(szPath, _T("r"));
	if (NULL == fp)
		return false;

	char readBuf[8192];
	FileReadStream is(fp, readBuf, sizeof(readBuf));
	fclose(fp);

	Document doc;
	doc.ParseStream(is);

	m_bDebugMode = doc["debugMode"].GetBool();
	const char* pszVer = doc["version"].GetString();
	const char* pszUrl = doc["updateUrl"].GetString();
	LogLevel logLevel = (LogLevel)doc["logLevel"].GetInt();
	const Value& tcpCfg = doc["tcpConfig"];
	m_tcpConfig.sPort = (unsigned short)tcpCfg["port"].GetInt();
	m_tcpConfig.nSockThreadCnt = tcpCfg["threadCount"].GetUint();
	m_tcpConfig.nMaxAcceptCnt = tcpCfg["acceptCount"].GetUint();
	m_tcpConfig.nMaxPackageSize = tcpCfg["pkgSize"].GetUint();

	EzLogger::setLogLevel(logLevel);

	m_server.setVersion(pszVer);
	m_server.setUpdUrl(pszUrl);

	return true;
}

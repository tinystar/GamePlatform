#include "DBServerMgr.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

using namespace rapidjson;

#define DB_CONFIG_FILE				_T("DBConfig.json")
#define DEF_DBSVR_PORT				5020
#define DEF_PACKAGE_SIZE			1024


DBServerMgr::DBServerMgr()
	: m_bDebugMode(false)
{
	m_tcpConfig.sPort = DEF_DBSVR_PORT;
	m_tcpConfig.nMaxPackageSize = DEF_PACKAGE_SIZE;
}

SVCErrorCode DBServerMgr::initServer()
{
	if (!EzVerify(loadConfig()))
		return eNotApplicable;

	ServerInitConfig initCfg;
	initCfg.tcpConfig = m_tcpConfig;
	return m_server.init(initCfg);
}

SVCErrorCode DBServerMgr::unInitServer()
{
	return m_server.unInit();
}

SVCErrorCode DBServerMgr::startServer()
{
	return m_server.start();
}

SVCErrorCode DBServerMgr::stopServer()
{
	return m_server.stop();
}

bool DBServerMgr::loadConfig()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	bool bRet = EzSys::getCurModulePath(szPath, MAX_PATH);
	if (!bRet)
		return false;

	size_t nLen = _tcslen(szPath);
	nLen += _tcslen(DB_CONFIG_FILE);
	if (nLen >= MAX_PATH)
		return false;

	_tcscat_s(szPath, MAX_PATH, DB_CONFIG_FILE);

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
	const char* pszDSN = doc["DSN"].GetString();
	const char* pszGuestName = doc["guestNamePrefix"].GetString();
	const char* pszGuestPW = doc["guestPassword"].GetString();
	int nHeadIndex = doc["headIndex"].GetInt();
	const char* pszGender = doc["defaultGender"].GetString();
	double dInitMoney = doc["initMoney"].GetDouble();
	unsigned int uInitRoomCard = doc["initRoomCard"].GetUint();
	const Value& tcpCfg = doc["tcpConfig"];
	m_tcpConfig.sPort = (unsigned short)tcpCfg["port"].GetInt();
	m_tcpConfig.nSockThreadCnt = tcpCfg["threadCount"].GetUint();
	m_tcpConfig.nMaxAcceptCnt = tcpCfg["acceptCount"].GetUint();
	m_tcpConfig.nMaxPackageSize = tcpCfg["pkgSize"].GetUint();

	int nGender = (0 == _stricmp(pszGender, "female")) ? 1 : 0;

	EzLogger::setLogLevel(logLevel);
	m_server.setDSN(pszDSN);
	m_server.setGuestName(pszGuestName);
	m_server.setGuestPassword(pszGuestPW);
	m_server.setDefHeadIdx(nHeadIndex);
	m_server.setDefGender(nGender);
	m_server.setInitMoney(dInitMoney);
	m_server.setInitRoomCard(uInitRoomCard);

	return true;
}

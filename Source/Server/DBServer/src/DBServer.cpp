#include "DBServer.h"
#include "DBMsgDefs.h"
#include "DBRecordSets.h"
#include "GameUserDefs.h"
#include "MainMsgDefs.h"


#define DEFAULT_INIT_MONEY		1000.0
#define DEFAULT_INIT_ROOMCARD	5
#define DEFAULT_HEAD_INDEX		eHeadDefault
#define DEFAULT_GENDER_TYPE		eFemale


NetMsgMapEntry DBServer::s_msgMapArray[] = {
	{ MSG_MAINID_DB, MSG_SUBID_CREATE_GUEST_ACCT, static_cast<NetMsgHandler>(&DBServer::onCreateGuestAccount) },
	{ MSG_MAINID_DB, MSG_SUBID_VALIDATE_ACCOUNT_LOGIN, static_cast<NetMsgHandler>(&DBServer::onValidateAcctLogin) },
	{ MSG_MAINID_DB, MSG_SUBID_VALIDATE_USERID_LOGIN, static_cast<NetMsgHandler>(&DBServer::onValidateUserIdLogin) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEKINDS, static_cast<NetMsgHandler>(&DBServer::onQueryGameKinds) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEPLACES, static_cast<NetMsgHandler>(&DBServer::onQueryGamePlaces) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEROOMS, static_cast<NetMsgHandler>(&DBServer::onQueryGameRooms) }
};


DBServer::DBServer()
	: m_pUIObserver(NULL)
	, m_nDefHeadIdx(DEFAULT_HEAD_INDEX)
	, m_nDefGender(DEFAULT_GENDER_TYPE)
	, m_dInitMoney(DEFAULT_INIT_MONEY)
	, m_uInitRoomCard(DEFAULT_INIT_ROOMCARD)
{
	::memset(m_szDSN, 0, sizeof(m_szDSN));
	::memset(m_szGuestName, 0, sizeof(m_szGuestName));
	::memset(m_szGuestPW, 0, sizeof(m_szGuestPW));
}

DBServer::~DBServer()
{

}

bool DBServer::onInit(const ServerInitConfig& serverConfig)
{
	if (!BaseGameServer::onInit(serverConfig))
		return false;

	BOOL bRet = false;
 	try
	{
		CString sDSN;
#ifdef _UNICODE
		const wchar_t* pszDSN = EzText::utf8ToWideChar(m_szDSN);
		sDSN = pszDSN;
		delete[] pszDSN;
#else
		sDSN = m_szDSN;
#endif

		bRet = m_database.Open(sDSN);
		if (bRet)
		{
			if (m_pUIObserver != NULL)
				m_pUIObserver->onUIDatabaseOpen(true, NULL);
		}
		else
		{
			if (m_pUIObserver != NULL)
				m_pUIObserver->onUIDatabaseOpen(false, NULL);
		}
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Open database error, error message: %s.\n"), pDBException->m_strError);
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIDatabaseOpen(false, pDBException->m_strError);
	}
	catch (...)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIDatabaseOpen(false, NULL);
	}

	registerMsgHandler(s_msgMapArray, EzCountOf(s_msgMapArray));
	return !!bRet;
}

bool DBServer::onUninit()
{
	try
	{
		if (m_database.IsOpen())
		{
			m_database.Close();
			if (m_pUIObserver != NULL)
				m_pUIObserver->onUIDatabaseClose();
		}
	}
	catch (...)
	{
	}

	removeMsgHandler(s_msgMapArray, EzCountOf(s_msgMapArray));
	return BaseGameServer::onUninit();
}

bool DBServer::onStart()
{
	return BaseGameServer::onStart();
}

bool DBServer::onStop()
{
	return BaseGameServer::onStop();
}

bool DBServer::setDSN(const char* pszDSN)
{
	if (NULL == pszDSN || strlen(pszDSN) > (sizeof(m_szDSN) - 1))
		return false;

	strcpy(m_szDSN, pszDSN);
	return true;
}

bool DBServer::setGuestName(const char* pszNamePrefix)
{
	if (NULL == pszNamePrefix || strlen(pszNamePrefix) > (sizeof(m_szGuestName) - 1))
		return false;

	strcpy(m_szGuestName, pszNamePrefix);
	return true;
}

bool DBServer::setGuestPassword(const char* pszPassword)
{
	if (NULL == pszPassword || strlen(pszPassword) > (sizeof(m_szGuestPW) - 1))
		return false;

	strcpy(m_szGuestPW, pszPassword);
	return true;
}

bool DBServer::setDefHeadIdx(int nIdx)
{
	if (nIdx < eHeadCustom)
		return false;

	m_nDefHeadIdx = nIdx;
	return true;
}

bool DBServer::setDefGender(int nGender)
{
	if (nGender != eMale || nGender != eFemale)
		return false;

	m_nDefGender = nGender;
	return true;
}

bool DBServer::setInitMoney(double dMoney)
{
	if (dMoney <= 0.0)
		return false;

	m_dInitMoney = dMoney;
	return true;
}

bool DBServer::setInitRoomCard(unsigned int uRoomCard)
{
	m_uInitRoomCard = uRoomCard;
	return true;
}

void DBServer::sendUserInfoMsg(ClientId id, CSUINT16 uSubMsgId, const ClientStamp& cliStamp, const UserInfoSet& userSet)
{
#ifdef _UNICODE
	const char* pszAccount = EzText::wideCharToUtf8(userSet.m_sAccount);
	const char* pszUserName = EzText::wideCharToUtf8(userSet.m_sUserName);
	const char* pszPhoneNum = EzText::wideCharToUtf8(userSet.m_sPhoneNum);
#else
	const char* pszAccount = userSet.m_sAccount.GetString();
	const char* pszUserName = userSet.m_sUserName.GetString();
	const char* pszPhoneNum = userSet.m_sPhoneNum.GetString();
#endif

	UserInfoWithClientMsg userMsg;
	userMsg.header.uMainId = MSG_MAINID_DB;
	userMsg.header.uSubId = uSubMsgId;
	userMsg.clientStamp = cliStamp;
	userMsg.userInfo.userId = (CSUINT32)userSet.m_nUserId;
	strncpy(userMsg.userInfo.szAccount, pszAccount, sizeof(userMsg.userInfo.szAccount) - 1);
	strncpy(userMsg.userInfo.szUserName, pszUserName, sizeof(userMsg.userInfo.szUserName) - 1);
	userMsg.userInfo.headIdx = userSet.m_nHeadIdx;
	userMsg.userInfo.genderType = userSet.m_nGenderType;
	userMsg.userInfo.uMoney = (CSUINT32)(userSet.m_dMoney * 100);	// 转为整形传输
	userMsg.userInfo.uRoomCard = userSet.m_nRoomCard;
	strncpy(userMsg.userInfo.szPhoneNum, pszPhoneNum, sizeof(userMsg.userInfo.szPhoneNum) - 1);
	userMsg.userInfo.uTypeFlag = userSet.m_nUserFlag;

#ifdef _UNICODE
	delete[] pszAccount;
	delete[] pszUserName;
	delete[] pszPhoneNum;
#endif

	sendMsg(id, &userMsg, sizeof(userMsg));
}

// DB服务器应该在消息处理函数中所有分支返回处理结果，否则可能造成其它服务器数据的堆积或者造成客户端的等待
void DBServer::onCreateGuestAccount(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(ClientStampMsg) == nDataLen);
	ClientStampMsg* pStampMsg = (ClientStampMsg*)pData;

	CRecordset rs(&m_database);
	CString sSql;
	CString sGuestName;
	CString sGuestPW;
#ifdef _UNICODE
	const wchar_t* pszGuestName = EzText::utf8ToWideChar(m_szGuestName);
	sGuestName = pszGuestName;
	delete[] pszGuestName;
	const wchar_t* pszGuestPW = EzText::utf8ToWideChar(m_szGuestPW);
	sGuestPW = pszGuestPW;
	delete[] pszGuestPW;
#else
	sGuestName = m_szGuestName;
	sGuestPW = m_szGuestPW;
#endif

	DBAcctLoginFailMsg loginFailMsg;
	loginFailMsg.header.uMainId = MSG_MAINID_DB;
	loginFailMsg.header.uSubId = MSG_SUBID_DB_LOGIN_FAILURE;
	loginFailMsg.clientStamp = pStampMsg->clientStamp;

	sSql.Format(_T("{CALL create_guest_account('%s', '%s', %d, %d, %.2f, %d, %d, ?)}"),
				sGuestName.GetString(),
				sGuestPW.GetString(),
				m_nDefHeadIdx,
				m_nDefGender,
				m_dInitMoney,
				m_uInitRoomCard,
				kGuest);

	TCHAR szRetBuf[20] = { 0 };
	SQLINTEGER nRetLen = 0;
	SQLRETURN sSQLRet = ::SQLBindParameter(rs.m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_CHAR, 16, 0, szRetBuf, 19, &nRetLen);
	if (SQL_SUCCESS != sSQLRet && SQL_SUCCESS_WITH_INFO != sSQLRet)
	{
		EzLogError(_T("Bind parameter failed in onCreateGuestAccount.\n"));
		loginFailMsg.nFailReason = eGuestCreateFail;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
		return;
	}

	try
	{
		BOOL bRet = rs.Open(CRecordset::snapshot, sSql, CRecordset::none);
		if (!bRet)
		{
			EzLogError(_T("Call create_guest_account failed.\n"));
			loginFailMsg.nFailReason = eGuestCreateFail;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		EzAssert(rs.GetODBCFieldCount() > 0);
		CString sRetGuest;
		rs.GetFieldValue((short)0, sRetGuest);
		if (sRetGuest.IsEmpty())
		{
			EzLogError(_T("create_guest_account execute failed, return value is empty.\n"));
			loginFailMsg.nFailReason = eGuestCreateFail;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		sSql.Format(_T("SELECT * FROM userinfo WHERE account = '%s'"), sRetGuest);
		UserInfoSet userSet(&m_database);
		if (!userSet.Open(CRecordset::snapshot, sSql) || userSet.IsEOF())
		{
			EzLogError(_T("Failed to get user information of %s.\n"), sRetGuest.GetString());
			loginFailMsg.nFailReason = eAccountNotExist;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		sendUserInfoMsg(id, MSG_SUBID_DB_LOGIN_SUCCESS, pStampMsg->clientStamp, userSet);
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onCreateGuestAccount, error message: %s.\n"), pDBException->m_strError);
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onCreateGuestAccount.\n"));
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
}

void DBServer::onValidateAcctLogin(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(ValidateAcctLoginMsg) == nDataLen);
	ValidateAcctLoginMsg* pValidateMsg = (ValidateAcctLoginMsg*)pData;

	CString sSql;
	CString sAccount;
	CString sPassword;
#ifdef _UNICODE
	const wchar_t* pszAccount = EzText::utf8ToWideChar(pValidateMsg->szAccount);
	sAccount = pszAccount;
	delete[] pszAccount;
	const wchar_t* pszPassword = EzText::utf8ToWideChar(pValidateMsg->szPassword);
	sPassword = pszPassword;
	delete[] pszPassword;
#else
	sAccount = pValidateMsg->szAccount;
	sPassword = pValidateMsg->szPassword;
#endif

	DBAcctLoginFailMsg loginFailMsg;
	loginFailMsg.header.uMainId = MSG_MAINID_DB;
	loginFailMsg.header.uSubId = MSG_SUBID_DB_LOGIN_FAILURE;
	loginFailMsg.clientStamp = pValidateMsg->clientStamp;

	try
	{
		sSql.Format(_T("SELECT * FROM userinfo WHERE account = '%s'"), sAccount);
		UserInfoSet userSet(&m_database);
		if (!userSet.Open(CRecordset::snapshot, sSql) || userSet.IsEOF())
		{
			loginFailMsg.nFailReason = eAccountNotExist;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		if (0 == userSet.m_sPassword.Compare(sPassword))
		{
			sendUserInfoMsg(id, MSG_SUBID_DB_LOGIN_SUCCESS, pValidateMsg->clientStamp, userSet);
		}
		else
		{
			loginFailMsg.nFailReason = ePasswordWrong;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
		}
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onValidateAcctLogin, error message: %s.\n"), pDBException->m_strError);
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onValidateAcctLogin.\n"));
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
}

void DBServer::onValidateUserIdLogin(ClientId id, void* pData, size_t nDataLen)
{

}

void DBServer::fillGameKindInfo(GameKindMsgInfo& kindInfo, const GameKindSet& kindSet)
{
#ifdef _UNICODE
	const char* pszGameName = EzText::wideCharToUtf8(kindSet.m_sGameName);
	const char* pszCliModName = EzText::wideCharToUtf8(kindSet.m_sClientModule);
	const char* pszSvrModName = EzText::wideCharToUtf8(kindSet.m_sServerModule);
	const char* pszVersion = EzText::wideCharToUtf8(kindSet.m_sGameVersion);
#else
	const char* pszGameName = kindSet.m_sGameName;
	const char* pszCliModName = kindSet.m_sClientModule;
	const char* pszSvrModName = kindSet.m_sServerModule;
	const char* pszVersion = kindSet.m_sGameVersion;
#endif

	kindInfo.nKindId = kindSet.m_nKindId;
	strncpy(kindInfo.szGameName, pszGameName, EzCountOf(kindInfo.szGameName) - 1);
	strncpy(kindInfo.szCliModInfo, pszCliModName, EzCountOf(kindInfo.szCliModInfo) - 1);
	strncpy(kindInfo.szSvrModInfo, pszSvrModName, EzCountOf(kindInfo.szSvrModInfo) - 1);
	strncpy(kindInfo.szVersion, pszVersion, EzCountOf(kindInfo.szVersion) - 1);

#ifdef _UNICODE
	delete[] pszGameName;
	delete[] pszCliModName;
	delete[] pszSvrModName;
	delete[] pszVersion;
#endif
}

void DBServer::onQueryGameKinds(ClientId id, void* pData, size_t nDataLen)
{
	GetGameInfoFailMsg failMsg;
	failMsg.header.uMainId = MSG_MAINID_DB;
	failMsg.header.uSubId = MSG_SUBID_GET_GAMEINFO_FAIL;

	try
	{
		long nKindCount = 0;
		CRecordset rsCount(&m_database);
		if (rsCount.Open(CRecordset::forwardOnly, _T("SELECT COUNT(*) FROM gamekind"), CRecordset::executeDirect))
		{
			CString sCount = _T("0");
			if (!rsCount.IsEOF())
				rsCount.GetFieldValue((short)0, sCount);
			nKindCount = _ttol(sCount);
			rsCount.Close();
		}

		if (0 == nKindCount)
		{
			EzLogInfo(_T("There is no game kind record in gamekind table.\n"));
			failMsg.nInfoType = kGameKind;
			strcpy(failMsg.szDetail, "There is no game kind record in gamekind table.");
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}

		GameKindSet kindSet(&m_database);
		if (!kindSet.Open(CRecordset::snapshot, _T("SELECT * FROM gamekind ORDER BY sortfield ASC")))
		{
			EzLogError(_T("Failed to get game kind list.\n"));
			failMsg.nInfoType = kGameKind;
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}

		size_t uMsgSize = sizeof(GameKindListMsg) + sizeof(GameKindMsgInfo) * (nKindCount - 1);
		GameKindListMsg* pKindListMsg = (GameKindListMsg*)::malloc(uMsgSize);
		if (NULL == pKindListMsg)
		{
			EzLogError(_T("Failed to allocate structure of GameKindListMsg.\n"));
			failMsg.nInfoType = kGameKind;
			strcpy(failMsg.szDetail, "Allocate message structure failure.");
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}
		::memset(pKindListMsg, 0, uMsgSize);

		pKindListMsg->header.uMainId = MSG_MAINID_DB;
		pKindListMsg->header.uSubId = MSG_SUBID_GET_GAMEKIND_SUCC;

		kindSet.MoveFirst();
		while (!kindSet.IsEOF())
		{
			fillGameKindInfo(pKindListMsg->kinds[pKindListMsg->uCount++], kindSet);
			kindSet.MoveNext();
		}
		EzAssert(pKindListMsg->uCount == nKindCount);

		sendMsg(id, pKindListMsg, uMsgSize);
		::free(pKindListMsg);
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onQueryGameKinds, error message: %s.\n"), pDBException->m_strError);
		failMsg.nInfoType = kGameKind;
		strcpy(failMsg.szDetail, "Unknown error has occured during fetch game kinds.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onQueryGameKinds.\n"));
		failMsg.nInfoType = kGameKind;
		strcpy(failMsg.szDetail, "Unknown error has occured during fetch game kinds.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
}

void DBServer::fillGamePlaceInfo(GamePlaceMsgInfo& placeInfo, const GamePlaceSet& placeSet)
{
#ifdef _UNICODE
	const char* pszPlaceName = EzText::wideCharToUtf8(placeSet.m_sPlaceName);
#else
	const char* pszPlaceName = placeSet.m_sPlaceName;
#endif

	placeInfo.nKindId = placeSet.m_nKindId;
	placeInfo.nPlaceId = placeSet.m_nPlaceId;
	strncpy(placeInfo.szPlaceName, pszPlaceName, EzCountOf(placeInfo.szPlaceName) - 1);
	placeInfo.nPlaceType = placeSet.m_nPlaceType;
	placeInfo.uEnterLimit = placeSet.m_dEnterLimit * 100;
	placeInfo.uBasePoint = placeSet.m_dBasePoint * 100;

#ifdef _UNICODE
	delete[] pszPlaceName;
#endif
}

void DBServer::onQueryGamePlaces(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(QueryGamePlaceMsg) == nDataLen);
	QueryGamePlaceMsg* pQueryPlaceMsg = (QueryGamePlaceMsg*)pData;

	GetGameInfoFailMsg failMsg;
	failMsg.header.uMainId = MSG_MAINID_DB;
	failMsg.header.uSubId = MSG_SUBID_GET_GAMEINFO_FAIL;

	try
	{
		long nPlaceCount = 0;
		CRecordset rsCount(&m_database);
		CString sSql;
		sSql.Format(_T("SELECT COUNT(*) FROM gameplace WHERE kindid = %d"), pQueryPlaceMsg->nKindId);
		if (rsCount.Open(CRecordset::forwardOnly, sSql, CRecordset::executeDirect))
		{
			CString sCount = _T("0");
			if (!rsCount.IsEOF())
				rsCount.GetFieldValue((short)0, sCount);
			nPlaceCount = _ttol(sCount);
			rsCount.Close();
		}

		if (0 == nPlaceCount)
		{
			EzLogInfo(_T("There is no game place record where kindid = %d.\n"), pQueryPlaceMsg->nKindId);
			failMsg.nInfoType = kGamePlace;
			strcpy(failMsg.szDetail, "There is no game place record.");
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}

		GamePlaceSet placeSet(&m_database);
		sSql.Format(_T("SELECT * FROM gameplace WHERE kindid = %d"), pQueryPlaceMsg->nKindId);
		if (!placeSet.Open(CRecordset::snapshot, sSql))
		{
			EzLogError(_T("Failed to get game place list.\n"));
			failMsg.nInfoType = kGamePlace;
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}

		size_t uMsgSize = sizeof(GamePlaceListMsg) + sizeof(GamePlaceMsgInfo) * (nPlaceCount - 1);
		GamePlaceListMsg* pPlaceListMsg = (GamePlaceListMsg*)::malloc(uMsgSize);
		if (NULL == pPlaceListMsg)
		{
			EzLogError(_T("Failed to allocate structure of GamePlaceListMsg.\n"));
			failMsg.nInfoType = kGamePlace;
			strcpy(failMsg.szDetail, "Allocate message structure failure.");
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}
		::memset(pPlaceListMsg, 0, uMsgSize);

		pPlaceListMsg->header.uMainId = MSG_MAINID_DB;
		pPlaceListMsg->header.uSubId = MSG_SUBID_GET_GAMEPLACE_SUCC;

		placeSet.MoveFirst();
		while (!placeSet.IsEOF())
		{
			fillGamePlaceInfo(pPlaceListMsg->places[pPlaceListMsg->uCount++], placeSet);
			placeSet.MoveNext();
		}
		EzAssert(pPlaceListMsg->uCount == nPlaceCount);

		sendMsg(id, pPlaceListMsg, uMsgSize);
		::free(pPlaceListMsg);
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onQueryGamePlaces, error message: %s.\n"), pDBException->m_strError);
		failMsg.nInfoType = kGamePlace;
		strcpy(failMsg.szDetail, "Unknown error has occured during fetch game places.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onQueryGamePlaces.\n"));
		failMsg.nInfoType = kGamePlace;
		strcpy(failMsg.szDetail, "Unknown error has occured during fetch game places.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
}

void DBServer::fillGameRoomInfo(GameRoomMsgInfo& roomInfo, const GameRoomSet& roomSet)
{
#ifdef _UNICODE
	const char* pszRoomName = EzText::wideCharToUtf8(roomSet.m_sRoomName);
	const char* pszServerIp = EzText::wideCharToUtf8(roomSet.m_sServerIp);
#else
	const char* pszRoomName = roomSet.m_sRoomName;
	const char* pszServerIp = roomSet.m_sServerIp;
#endif

	roomInfo.nKindId = roomSet.m_nKindId;
	roomInfo.nPlaceId = roomSet.m_nPlaceId;
	roomInfo.nRoomId = roomSet.m_nRoomId;
	strncpy(roomInfo.szRoomName, pszRoomName, EzCountOf(roomInfo.szRoomName) - 1);
	strncpy(roomInfo.szServerIp, pszServerIp, EzCountOf(roomInfo.szServerIp) - 1);
	roomInfo.sServerPort = (CSUINT16)roomSet.m_nServerPort;

#ifdef _UNICODE
	delete[] pszRoomName;
	delete[] pszServerIp;
#endif
}

void DBServer::onQueryGameRooms(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(QueryGameRoomMsg) == nDataLen);
	QueryGameRoomMsg* pQueryRoomMsg = (QueryGameRoomMsg*)pData;

	GetGameInfoFailMsg failMsg;
	failMsg.header.uMainId = MSG_MAINID_DB;
	failMsg.header.uSubId = MSG_SUBID_GET_GAMEINFO_FAIL;

	try
	{
		long nRoomCount = 0;
		CRecordset rsCount(&m_database);
		CString sSql;
		sSql.Format(_T("SELECT COUNT(*) FROM gameroom WHERE kindid = %d AND placeid = %d"), pQueryRoomMsg->nKindId, pQueryRoomMsg->nPlaceId);
		if (rsCount.Open(CRecordset::forwardOnly, sSql, CRecordset::executeDirect))
		{
			CString sCount = _T("0");
			if (!rsCount.IsEOF())
				rsCount.GetFieldValue((short)0, sCount);
			nRoomCount = _ttol(sCount);
			rsCount.Close();
		}

		if (0 == nRoomCount)
		{
			EzLogInfo(_T("There is no game room record where kindid = %d and placeid = %d.\n"), pQueryRoomMsg->nKindId, pQueryRoomMsg->nPlaceId);
			failMsg.nInfoType = kGameRoom;
			strcpy(failMsg.szDetail, "There is no game room record.");
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}

		GameRoomSet roomSet(&m_database);
		sSql.Format(_T("SELECT * FROM gameroom WHERE kindid = %d AND placeid = %d"), pQueryRoomMsg->nKindId, pQueryRoomMsg->nPlaceId);
		if (!roomSet.Open(CRecordset::snapshot, sSql))
		{
			EzLogError(_T("Failed to get game room list.\n"));
			failMsg.nInfoType = kGameRoom;
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}

		size_t uMsgSize = sizeof(GameRoomListMsg) + sizeof(GameRoomMsgInfo) * (nRoomCount - 1);
		GameRoomListMsg* pRoomListMsg = (GameRoomListMsg*)::malloc(uMsgSize);
		if (NULL == pRoomListMsg)
		{
			EzLogError(_T("Failed to allocate structure of GameRoomListMsg.\n"));
			failMsg.nInfoType = kGameRoom;
			strcpy(failMsg.szDetail, "Allocate message structure failure.");
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}
		::memset(pRoomListMsg, 0, uMsgSize);

		pRoomListMsg->header.uMainId = MSG_MAINID_DB;
		pRoomListMsg->header.uSubId = MSG_SUBID_GET_GAMEROOM_SUCC;

		roomSet.MoveFirst();
		while (!roomSet.IsEOF())
		{
			fillGameRoomInfo(pRoomListMsg->rooms[pRoomListMsg->uCount++], roomSet);
			roomSet.MoveNext();
		}
		EzAssert(pRoomListMsg->uCount == nRoomCount);

		sendMsg(id, pRoomListMsg, uMsgSize);
		::free(pRoomListMsg);
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onQueryGameRooms, error message: %s.\n"), pDBException->m_strError);
		failMsg.nInfoType = kGameRoom;
		strcpy(failMsg.szDetail, "Unknown error has occured during fetch game rooms.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onQueryGameRooms.\n"));
		failMsg.nInfoType = kGameRoom;
		strcpy(failMsg.szDetail, "Unknown error has occured during fetch game rooms.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
}

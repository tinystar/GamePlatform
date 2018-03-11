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
	{ MSG_MAINID_DB, MSG_SUBID_LOGIN_MAIN_BY_ACCOUNT, static_cast<NetMsgHandler>(&DBServer::onLoginMainByAccount) },
	{ MSG_MAINID_DB, MSG_SUBID_LOGIN_MAIN_BY_USERID, static_cast<NetMsgHandler>(&DBServer::onLoginMainByUserId) },
	{ MSG_MAINID_DB, MSG_SUBID_LOGOUT_MAIN, static_cast<NetMsgHandler>(&DBServer::onUserLogoutMain) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEKINDS, static_cast<NetMsgHandler>(&DBServer::onQueryGameKinds) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEPLACES, static_cast<NetMsgHandler>(&DBServer::onQueryGamePlaces) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEROOMS, static_cast<NetMsgHandler>(&DBServer::onQueryGameRooms) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_PLACES_END, static_cast<NetMsgHandler>(&DBServer::onQueryPlacesEnd) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_ROOMS_END, static_cast<NetMsgHandler>(&DBServer::onQueryRoomsEnd) }
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
		EzString sDSNConv(m_szDSN, kUtf8);
		CString sDSN = sDSNConv.kwcharPtr();

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
	EzString sAccount(userSet.m_sAccount);
	EzString sUserName(userSet.m_sUserName);
	EzString sPhoneNum(userSet.m_sPhoneNum);

	UserInfoWithClientMsg userMsg;
	userMsg.header.uMainId = MSG_MAINID_DB;
	userMsg.header.uSubId = uSubMsgId;
	userMsg.clientStamp = cliStamp;
	userMsg.userInfo.userId = (CSUINT32)userSet.m_nUserId;
	strncpy(userMsg.userInfo.szAccount, sAccount.kcharPtr(kUtf8), sizeof(userMsg.userInfo.szAccount) - 1);
	strncpy(userMsg.userInfo.szUserName, sUserName.kcharPtr(kUtf8), sizeof(userMsg.userInfo.szUserName) - 1);
	userMsg.userInfo.headIdx = userSet.m_nHeadIdx;
	userMsg.userInfo.genderType = userSet.m_nGenderType;
	userMsg.userInfo.uMoney = (CSUINT32)(userSet.m_dMoney * 100);	// 转为整形传输
	userMsg.userInfo.uRoomCard = userSet.m_nRoomCard;
	strncpy(userMsg.userInfo.szPhoneNum, sPhoneNum.kcharPtr(kUtf8), sizeof(userMsg.userInfo.szPhoneNum) - 1);
	userMsg.userInfo.uTypeFlag = userSet.m_nUserFlag;

	sendMsg(id, &userMsg, sizeof(userMsg));
}

// DB服务器应该在消息处理函数中所有分支返回处理结果，否则可能造成其它服务器数据的堆积或者造成客户端的等待
void DBServer::onCreateGuestAccount(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(CreateGuestAccountMsg) == nDataLen);
	CreateGuestAccountMsg* pCreateGuestMsg = (CreateGuestAccountMsg*)pData;

	EzString sNameConvt(m_szGuestName, kUtf8);
	EzString sPWConvt(m_szGuestPW, kUtf8);

	CRecordset rs(&m_database);
	CString sSql;
	CString sGuestName = sNameConvt.kwcharPtr();
	CString sGuestPW = sPWConvt.kwcharPtr();

	DBAcctLoginFailMsg loginFailMsg;
	loginFailMsg.header.uMainId = MSG_MAINID_DB;
	loginFailMsg.header.uSubId = MSG_SUBID_DB_LOGIN_FAILURE;
	loginFailMsg.clientStamp = pCreateGuestMsg->clientStamp;

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

		EzString sGuestUtf8(sRetGuest.GetString());

		ValidateLoginMainByAcctMsg validateMsg;
		validateMsg.header.uMainId = MSG_MAINID_DB;
		validateMsg.header.uSubId = MSG_SUBID_LOGIN_MAIN_BY_ACCOUNT;
		validateMsg.clientStamp = pCreateGuestMsg->clientStamp;
		strncpy(validateMsg.szAccount, sGuestUtf8.kcharPtr(kUtf8), sizeof(validateMsg.szAccount) - 1);
		validateMsg.info = pCreateGuestMsg->info;
		memcpy(validateMsg.info.szPassword, m_szGuestPW, sizeof(validateMsg.info.szPassword));

		onLoginMainByAccount(id, &validateMsg, sizeof(validateMsg));
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

void DBServer::onLoginMainByAccount(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(ValidateLoginMainByAcctMsg) == nDataLen);
	ValidateLoginMainByAcctMsg* pValidateMsg = (ValidateLoginMainByAcctMsg*)pData;

	CString sSql;
	EzString sAcctConvt(pValidateMsg->szAccount, kUtf8);
	CString sAccount = sAcctConvt.kwcharPtr();

	DBAcctLoginFailMsg loginFailMsg;
	loginFailMsg.header.uMainId = MSG_MAINID_DB;
	loginFailMsg.header.uSubId = MSG_SUBID_DB_LOGIN_FAILURE;
	loginFailMsg.clientStamp = pValidateMsg->clientStamp;

	try
	{
		CRecordset rsUserId(&m_database);
		sSql.Format(_T("SELECT userid FROM TUserInfo WHERE account = '%s'"), sAccount);
		if (!rsUserId.Open(CRecordset::forwardOnly, sSql, CRecordset::executeDirect) || rsUserId.IsEOF())
		{
			loginFailMsg.nFailReason = eAccountNotExist;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		CString strUserId = _T("0");
		rsUserId.GetFieldValue((short)0, strUserId);
		CSUINT32 uUserId = _ttoi(strUserId);

		ValidateLoginMainByIdMsg validateByIdMsg;
		validateByIdMsg.header.uMainId = MSG_MAINID_DB;
		validateByIdMsg.header.uSubId = MSG_SUBID_LOGIN_MAIN_BY_USERID;
		validateByIdMsg.clientStamp = pValidateMsg->clientStamp;
		validateByIdMsg.userId = uUserId;
		validateByIdMsg.info = pValidateMsg->info;

		onLoginMainByUserId(id, &validateByIdMsg, sizeof(validateByIdMsg));
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onLoginMainByAccount, error message: %s.\n"), pDBException->m_strError);
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onLoginMainByAccount.\n"));
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
}

/**
* 0	-	登录成功
* 1	-	用户不存在
* 2	-	已经登录
* 3 -	密码不正确
* 4 -	内部错误
*/
CSINT32 DBServer::ValidateResultToFailReason(int result) const
{
	switch (result)
	{
	case 1:
		return eAccountNotExist;
	case 2:
		return eAlreadyLogin;
	case 3:
		return ePasswordWrong;
	case 4:
	default:
		return eUnknownReason;
	}

	return eUnknownReason;
}

void DBServer::onLoginMainByUserId(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(ValidateLoginMainByIdMsg) == nDataLen);
	ValidateLoginMainByIdMsg* pValidateMsg = (ValidateLoginMainByIdMsg*)pData;

	CString sSql;
	EzString sPWConvt(pValidateMsg->info.szPassword, kUtf8);
	EzString sSvrNameConvt(pValidateMsg->info.szServerName, kUtf8);
	EzString sOSConvt(pValidateMsg->info.szOS, kUtf8);
	EzString sDeviceConvt(pValidateMsg->info.szDevice, kUtf8);
	EzString sLoginIpConvt(pValidateMsg->info.szLoginIp, kUtf8);
	CString sPassword = sPWConvt.kwcharPtr();
	CString sSvrName = sSvrNameConvt.kwcharPtr();
	CString sOS = sOSConvt.kwcharPtr();
	CString sDevice = sDeviceConvt.kwcharPtr();
	CString sLoginIp = sLoginIpConvt.kwcharPtr();

	DBAcctLoginFailMsg loginFailMsg;
	loginFailMsg.header.uMainId = MSG_MAINID_DB;
	loginFailMsg.header.uSubId = MSG_SUBID_DB_LOGIN_FAILURE;
	loginFailMsg.clientStamp = pValidateMsg->clientStamp;

	sSql.Format(_T("{CALL validate_main_login_by_userid(%d, '%s', '%s', '%s', '%s', '%s', ?)}"),
				pValidateMsg->userId,
				sPassword,
				sSvrName,
				sLoginIp,
				sOS,
				sDevice);

	int nResult = -1;
	SQLINTEGER nRetLen = 0;
	CRecordset rs(&m_database);
	SQLRETURN sSQLRet = ::SQLBindParameter(rs.m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nResult, 4, &nRetLen);
	if (SQL_SUCCESS != sSQLRet && SQL_SUCCESS_WITH_INFO != sSQLRet)
	{
		EzLogError(_T("Bind parameter failed in onLoginMainByUserId.\n"));
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
		return;
	}

	try
	{
		BOOL bRet = rs.Open(CRecordset::snapshot, sSql, CRecordset::none);
		if (!bRet)
		{
			EzLogError(_T("Call validate_main_login_by_userid failed.\n"));
			loginFailMsg.nFailReason = eUnknownReason;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		EzAssert(rs.GetODBCFieldCount() > 0);
		CString sExecResult;
		rs.GetFieldValue((short)0, sExecResult);
		int nExecRet = _ttoi(sExecResult);

		if (nExecRet != 0)		// error occurred
		{
			loginFailMsg.nFailReason = ValidateResultToFailReason(nExecRet);
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		UserInfoSet userSet(&m_database);
		sSql.Format(_T("SELECT * FROM TUserInfo WHERE userid = %d"), pValidateMsg->userId);
		if (!userSet.Open(CRecordset::snapshot, sSql) || userSet.IsEOF())
		{
			loginFailMsg.nFailReason = eAccountNotExist;
			sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
			return;
		}

		sendUserInfoMsg(id, MSG_SUBID_DB_LOGIN_SUCCESS, pValidateMsg->clientStamp, userSet);
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onLoginMainByUserId, error message: %s.\n"), pDBException->m_strError);
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onLoginMainByUserId.\n"));
		loginFailMsg.nFailReason = eUnknownReason;
		sendMsg(id, &loginFailMsg, sizeof(loginFailMsg));
	}
}

void DBServer::onUserLogoutMain(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(UserLogoutMainMsg) == nDataLen);
	UserLogoutMainMsg* pLogoutMsg = (UserLogoutMainMsg*)pData;

	CString sSql;
	sSql.Format(_T("{CALL delete_main_login_record(%d)}"), pLogoutMsg->userId);

	try
	{
		m_database.ExecuteSQL(sSql);
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onUserLogoutMain, error message: %s.\n"), pDBException->m_strError);
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onUserLogoutMain.\n"));
	}
}

void DBServer::fillGameKindInfo(GameKindMsgInfo& kindInfo, const GameKindSet& kindSet)
{
	EzString sGameName(kindSet.m_sGameName);
	EzString sCliModName(kindSet.m_sClientModule);
	EzString sSvrModName(kindSet.m_sServerModule);
	EzString sVersion(kindSet.m_sGameVersion);

	kindInfo.nKindId = kindSet.m_nKindId;
	strncpy(kindInfo.szGameName, sGameName.kcharPtr(kUtf8), EzCountOf(kindInfo.szGameName) - 1);
	strncpy(kindInfo.szCliModInfo, sCliModName.kcharPtr(kUtf8), EzCountOf(kindInfo.szCliModInfo) - 1);
	strncpy(kindInfo.szSvrModInfo, sSvrModName.kcharPtr(kUtf8), EzCountOf(kindInfo.szSvrModInfo) - 1);
	strncpy(kindInfo.szVersion, sVersion.kcharPtr(kUtf8), EzCountOf(kindInfo.szVersion) - 1);
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
		if (rsCount.Open(CRecordset::forwardOnly, _T("SELECT COUNT(*) FROM TGameKind"), CRecordset::executeDirect))
		{
			CString sCount = _T("0");
			if (!rsCount.IsEOF())
				rsCount.GetFieldValue((short)0, sCount);
			nKindCount = _ttol(sCount);
			rsCount.Close();
		}

		if (0 == nKindCount)
		{
			EzLogInfo(_T("There is no game kind record in TGameKind table.\n"));
			failMsg.nInfoType = kGameKind;
			strcpy(failMsg.szDetail, "There is no game kind record in TGameKind table.");
			sendMsg(id, &failMsg, sizeof(failMsg));
			return;
		}

		GameKindSet kindSet(&m_database);
		if (!kindSet.Open(CRecordset::snapshot, _T("SELECT * FROM TGameKind ORDER BY sortfield ASC")))
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
	EzString sPlaceName(placeSet.m_sPlaceName);

	placeInfo.nKindId = placeSet.m_nKindId;
	placeInfo.nPlaceId = placeSet.m_nPlaceId;
	strncpy(placeInfo.szPlaceName, sPlaceName.kcharPtr(kUtf8), EzCountOf(placeInfo.szPlaceName) - 1);
	placeInfo.nPlaceType = placeSet.m_nPlaceType;
	placeInfo.uEnterLimit = (CSUINT32)placeSet.m_dEnterLimit * 100;
	placeInfo.uBasePoint = (CSUINT32)placeSet.m_dBasePoint * 100;
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
		sSql.Format(_T("SELECT COUNT(*) FROM TGamePlace WHERE kindid = %d"), pQueryPlaceMsg->nKindId);
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
		sSql.Format(_T("SELECT * FROM TGamePlace WHERE kindid = %d"), pQueryPlaceMsg->nKindId);
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
	EzString sRoomName(roomSet.m_sRoomName);
	EzString sServerIp(roomSet.m_sServerIp);

	roomInfo.nKindId = roomSet.m_nKindId;
	roomInfo.nPlaceId = roomSet.m_nPlaceId;
	roomInfo.nRoomId = roomSet.m_nRoomId;
	strncpy(roomInfo.szRoomName, sRoomName.kcharPtr(kUtf8), EzCountOf(roomInfo.szRoomName) - 1);
	strncpy(roomInfo.szServerIp, sServerIp.kcharPtr(kUtf8), EzCountOf(roomInfo.szServerIp) - 1);
	roomInfo.sServerPort = (CSUINT16)roomSet.m_nServerPort;
	roomInfo.uMaxUser = roomSet.m_nMaxUserCnt;
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
		sSql.Format(_T("SELECT COUNT(*) FROM TGameRoom WHERE kindid = %d AND placeid = %d"), pQueryRoomMsg->nKindId, pQueryRoomMsg->nPlaceId);
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
		sSql.Format(_T("SELECT * FROM TGameRoom WHERE kindid = %d AND placeid = %d"), pQueryRoomMsg->nKindId, pQueryRoomMsg->nPlaceId);
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
		strcpy(failMsg.szDetail, "Unknown error has occurred during fetch game rooms.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onQueryGameRooms.\n"));
		failMsg.nInfoType = kGameRoom;
		strcpy(failMsg.szDetail, "Unknown error has occurred during fetch game rooms.");
		sendMsg(id, &failMsg, sizeof(failMsg));
	}
}

void DBServer::onQueryPlacesEnd(ClientId id, void* pData, size_t nDataLen)
{
	sendMsg(id, pData, nDataLen);
}

void DBServer::onQueryRoomsEnd(ClientId id, void* pData, size_t nDataLen)
{
	sendMsg(id, pData, nDataLen);
}

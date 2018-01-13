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
		if (!userSet.Open(CRecordset::snapshot, sSql) ||
			userSet.GetRowsFetched() <= 0)
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
		if (!userSet.Open(CRecordset::snapshot, sSql) ||
			userSet.GetRowsFetched() <= 0)
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

void DBServer::onQueryGameKinds(ClientId id, void* pData, size_t nDataLen)
{
	try
	{
		GameKindSet kindSet(&m_database);
		if (!kindSet.Open(CRecordset::snapshot, _T("SELECT * FROM gamekind ORDER BY sortweight DESC")))
		{
			EzLogError(_T("Failed to execute sql that query game kinds from gamekind table.\n"));
			return;
		}

		if (kindSet.GetRowsFetched() <= 0)
			return;

		kindSet.MoveFirst();
		while (!kindSet.IsEOF())
		{
			kindSet.MoveNext();
		}
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onQueryGameKinds, error message: %s.\n"), pDBException->m_strError);
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onQueryGameKinds.\n"));
	}
}

void DBServer::onQueryGamePlaces(ClientId id, void* pData, size_t nDataLen)
{

}

void DBServer::onQueryGameRooms(ClientId id, void* pData, size_t nDataLen)
{

}

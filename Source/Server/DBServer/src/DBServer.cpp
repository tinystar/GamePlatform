#include "DBServer.h"
#include "DBMsgDefs.h"
#include "DBRecordSets.h"
#include "GameUserDefs.h"


#define DEFAULT_INIT_MONEY		1000.0
#define DEFAULT_INIT_ROOMCARD	5


NetMsgMapEntry DBServer::s_msgMapArray[] = {
	{ MSG_MAINID_DB, MSG_SUBID_CREATE_GUEST_ACCT, static_cast<NetMsgHandler>(&DBServer::onCreateGuestAccount) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEKINDS, static_cast<NetMsgHandler>(&DBServer::onQueryGameKinds) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEPLACES, static_cast<NetMsgHandler>(&DBServer::onQueryGamePlaces) },
	{ MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEROOMS, static_cast<NetMsgHandler>(&DBServer::onQueryGameRooms) }
};


DBServer::DBServer()
	: m_pUIObserver(NULL)
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

	sSql.Format(_T("{CALL create_guest_account('%s', '%s', %.2f, %d, %d, ?)}"),
				sGuestName.GetString(),
				sGuestPW.GetString(),
				m_dInitMoney,
				m_uInitRoomCard,
				kGuest);

	TCHAR szRetBuf[20] = { 0 };
	SQLINTEGER nRetLen = 0;
	SQLRETURN sSQLRet = ::SQLBindParameter(rs.m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_CHAR, 16, 0, szRetBuf, 19, &nRetLen);
	if (SQL_SUCCESS != sSQLRet && SQL_SUCCESS_WITH_INFO != sSQLRet)
	{
		EzLogError(_T("Bind parameter failed in onCreateGuestAccount.\n"));
		pStampMsg->header.uSubId = MSG_SUBID_GUEST_CREATE_FAIL;
		sendMsg(id, pStampMsg, sizeof(ClientStampMsg));
		return;
	}

	try
	{
		BOOL bRet = rs.Open(CRecordset::snapshot, sSql, CRecordset::none);
		if (!bRet)
		{
			EzLogError(_T("Call create_guest_account failed.\n"));
			pStampMsg->header.uSubId = MSG_SUBID_GUEST_CREATE_FAIL;
			sendMsg(id, pStampMsg, sizeof(ClientStampMsg));
			return;
		}

		EzAssert(rs.GetODBCFieldCount() > 0);
		CString sRetGuest;
		rs.GetFieldValue((short)0, sRetGuest);
		if (sRetGuest.IsEmpty())
		{
			EzLogError(_T("create_guest_account execute failed, return value is empty.\n"));
			pStampMsg->header.uSubId = MSG_SUBID_GUEST_CREATE_FAIL;
			sendMsg(id, pStampMsg, sizeof(ClientStampMsg));
			return;
		}

		sSql.Format(_T("SELECT * FROM userinfo WHERE account = '%s'"), sRetGuest);
		UserInfoSet userSet(&m_database);
		if (!userSet.Open(CRecordset::snapshot, sSql))
		{
			EzLogError(_T("Failed to get user information of %s.\n"), sRetGuest.GetString());
			pStampMsg->header.uSubId = MSG_SUBID_GUEST_CREATE_FAIL;
			sendMsg(id, pStampMsg, sizeof(ClientStampMsg));
			return;
		}

		if (userSet.GetRowsFetched() <= 0)
		{
			pStampMsg->header.uSubId = MSG_SUBID_GUEST_CREATE_FAIL;
			sendMsg(id, pStampMsg, sizeof(ClientStampMsg));
			return;
		}

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
		userMsg.header.uSubId = MSG_SUBID_GUEST_CREATE_SUCC;
		userMsg.clientId = pStampMsg->clientId;
		userMsg.ulStamp = pStampMsg->ulStamp;
		userMsg.userId = userSet.m_nUserId;
		strncpy(userMsg.szAccount, pszAccount, sizeof(userMsg.szAccount) - 1);
		strncpy(userMsg.szUserName, pszUserName, sizeof(userMsg.szUserName) - 1);
		userMsg.genderType = userSet.m_nGenderType;
		userMsg.uMoney = (CSUINT32)(userSet.m_dMoney * 100);	// 转为整形传输
		userMsg.uRoomCard = userSet.m_nRoomCard;
		strncpy(userMsg.szPhoneNum, pszPhoneNum, sizeof(userMsg.szPhoneNum) - 1);
		userMsg.uTypeFlag = userSet.m_nUserFlag;

#ifdef _UNICODE
		delete[] pszAccount;
		delete[] pszUserName;
		delete[] pszPhoneNum;
#endif

		sendMsg(id, &userMsg, sizeof(userMsg));
	}
	catch (CDBException* pDBException)
	{
		EzLogError(_T("Error occur in onCreateGuestAccount, error message: %s.\n"), pDBException->m_strError);
		pStampMsg->header.uSubId = MSG_SUBID_GUEST_CREATE_FAIL;
		sendMsg(id, pStampMsg, sizeof(ClientStampMsg));
	}
	catch (...)
	{
		EzLogError(_T("Unknown error occur in onCreateGuestAccount.\n"));
		pStampMsg->header.uSubId = MSG_SUBID_GUEST_CREATE_FAIL;
		sendMsg(id, pStampMsg, sizeof(ClientStampMsg));
	}
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

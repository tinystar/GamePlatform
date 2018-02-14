#include "DBRecordSets.h"
#include "GameUserDefs.h"


//-------------------------------------------------------------------------------
// UserInfoSet
//-------------------------------------------------------------------------------
UserInfoSet::UserInfoSet(CDatabase* pDatabase /*= NULL*/)
	: CRecordset(pDatabase)
	, m_nUserId(0)
	, m_nHeadIdx(0)
	, m_nGenderType(0)
	, m_dMoney(0.0)
	, m_nRoomCard(0)
	, m_nUserFlag(0)
{
	m_nFields = 10;
}

void UserInfoSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("userid"), m_nUserId);
	RFX_Text(pFX, _T("account"), m_sAccount);
	RFX_Text(pFX, _T("password"), m_sPassword);
	RFX_Text(pFX, _T("username"), m_sUserName);
	RFX_Int(pFX, _T("headIndex"), m_nHeadIdx);
	RFX_Int(pFX, _T("gender"), m_nGenderType);
	RFX_Double(pFX, _T("money"), m_dMoney);
	RFX_Int(pFX, _T("roomcardcount"), m_nRoomCard);
	RFX_Text(pFX, _T("phonenumber"), m_sPhoneNum);
	RFX_Int(pFX, _T("flag"), m_nUserFlag);
}


//-------------------------------------------------------------------------------
// GameKindSet
//-------------------------------------------------------------------------------
GameKindSet::GameKindSet(CDatabase* pDatabase /*= NULL*/)
	: CRecordset(pDatabase)
	, m_nKindId(-1)
	, m_nSortField(0)
{
	m_nFields = 6;
}

void GameKindSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("kindid"), m_nKindId);
	RFX_Text(pFX, _T("gamename"), m_sGameName);
	RFX_Text(pFX, _T("clientmodule"), m_sClientModule);
	RFX_Text(pFX, _T("servermodule"), m_sServerModule);
	RFX_Text(pFX, _T("version"), m_sGameVersion);
	RFX_Int(pFX, _T("sortfield"), m_nSortField);
}


//-------------------------------------------------------------------------------
// GamePlaceSet
//-------------------------------------------------------------------------------
GamePlaceSet::GamePlaceSet(CDatabase* pDatabase /*= NULL*/)
	: CRecordset(pDatabase)
	, m_nKindId(-1)
	, m_nPlaceId(-1)
	, m_nPlaceType(0)
	, m_dEnterLimit(0.0)
	, m_dBasePoint(0.0)
{
	m_nFields = 6;
}

void GamePlaceSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("kindid"), m_nKindId);
	RFX_Int(pFX, _T("placeid"), m_nPlaceId);
	RFX_Text(pFX, _T("placename"), m_sPlaceName);
	RFX_Int(pFX, _T("placetype"), m_nPlaceType);
	RFX_Double(pFX, _T("enterlimit"), m_dEnterLimit);
	RFX_Double(pFX, _T("basepoint"), m_dBasePoint);
}


//-------------------------------------------------------------------------------
// GameRoomSet
//-------------------------------------------------------------------------------
GameRoomSet::GameRoomSet(CDatabase* pDatabase /*= NULL*/)
	: CRecordset(pDatabase)
	, m_nKindId(-1)
	, m_nPlaceId(-1)
	, m_nRoomId(-1)
	, m_nServerPort(0)
	, m_nMaxUserCnt(0)
{
	m_nFields = 7;
}

void GameRoomSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("kindid"), m_nKindId);
	RFX_Int(pFX, _T("placeid"), m_nPlaceId);
	RFX_Int(pFX, _T("roomid"), m_nRoomId);
	RFX_Text(pFX, _T("roomname"), m_sRoomName);
	RFX_Text(pFX, _T("serverip"), m_sServerIp);
	RFX_Int(pFX, _T("serverport"), m_nServerPort);
	RFX_Int(pFX, _T("maxuser"), m_nMaxUserCnt);
}

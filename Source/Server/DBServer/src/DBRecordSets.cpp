#include "DBRecordSets.h"
#include "GameUserDefs.h"


//-------------------------------------------------------------------------------
// UserInfoSet
//-------------------------------------------------------------------------------
UserInfoSet::UserInfoSet(CDatabase* pDatabase /*= NULL*/)
	: CRecordset(pDatabase)
	, m_nUserId(0)
	, m_nGenderType(0)
	, m_dMoney(0.0)
	, m_nRoomCard(0)
	, m_nUserFlag(0)
{
	m_nFields = 9;
}

void UserInfoSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("userid"), m_nUserId);
	RFX_Text(pFX, _T("account"), m_sAccount);
	RFX_Text(pFX, _T("password"), m_sPassword);
	RFX_Text(pFX, _T("username"), m_sUserName);
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
	, m_nSortWeight(0)
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
	RFX_Int(pFX, _T("sortweight"), m_nSortWeight);
}

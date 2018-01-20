/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/12/20
/************************************************************************/
#ifndef __DB_RECORDSETS_H__
#define __DB_RECORDSETS_H__

#include <afxdb.h>


class UserInfoSet : public CRecordset
{
public:
	explicit UserInfoSet(CDatabase* pDatabase = NULL);

	int		m_nUserId;
	CString	m_sAccount;
	CString	m_sPassword;
	CString	m_sUserName;
	int		m_nHeadIdx;
	int		m_nGenderType;
	double	m_dMoney;
	int		m_nRoomCard;
	CString	m_sPhoneNum;
	int		m_nUserFlag;

	virtual void DoFieldExchange(CFieldExchange* pFX);
};

class GameKindSet : public CRecordset
{
public:
	explicit GameKindSet(CDatabase* pDatabase = NULL);

	int		m_nKindId;
	CString	m_sGameName;
	CString	m_sClientModule;
	CString	m_sServerModule;
	CString	m_sGameVersion;
	int		m_nSortField;

	virtual void DoFieldExchange(CFieldExchange* pFX);
};

class GamePlaceSet : public CRecordset
{
public:
	explicit GamePlaceSet(CDatabase* pDatabase = NULL);

	int		m_nKindId;
	int		m_nPlaceId;
	CString	m_sPlaceName;
	int		m_nPlaceType;
	double	m_dEnterLimit;
	double	m_dBasePoint;

	virtual void DoFieldExchange(CFieldExchange* pFX);
};

class GameRoomSet : public CRecordset
{
public:
	explicit GameRoomSet(CDatabase* pDatabase = NULL);

	int		m_nKindId;
	int		m_nPlaceId;
	int		m_nRoomId;
	CString	m_sRoomName;
	CString m_sServerIp;
	int		m_nServerPort;

	virtual void DoFieldExchange(CFieldExchange* pFX);
};

#endif // __DB_RECORDSETS_H__

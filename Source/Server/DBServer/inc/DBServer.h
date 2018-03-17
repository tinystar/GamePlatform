/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/9/4
/************************************************************************/
#ifndef __DB_SERVER_H__
#define __DB_SERVER_H__

#include <afxdb.h>
#include "BaseGameServer.h"
#include "IDBServerUIObserver.h"
#include "DBRecordSets.h"
#include "BaseMsgDefs.h"

class DBServer : public BaseGameServer
{
public:
	DBServer();
	virtual ~DBServer();

public:
	bool setDSN(const char* pszDSN);
	bool setGuestName(const char* pszNamePrefix);
	bool setGuestPassword(const char* pszPassword);
	bool setDefHeadIdx(int nIdx);
	bool setDefGender(int nGender);
	bool setInitMoney(double dMoney);
	bool setInitRoomCard(unsigned int uRoomCard);

	void registerUIObserver(IDBServerUIObserver* pObserver) { m_pUIObserver = pObserver; }

protected:
	virtual bool onInit(const ServerInitConfig& serverConfig);
	virtual bool onUninit();

protected:
	void onCreateGuestAccount(ClientId id, void* pData, size_t nDataLen);
	void onLoginMainByAccount(ClientId id, void* pData, size_t nDataLen);
	void onLoginMainByUserId(ClientId id, void* pData, size_t nDataLen);

	void onUserLogoutMain(ClientId id, void* pData, size_t nDataLen);

	void onQueryGameKinds(ClientId id, void* pData, size_t nDataLen);
	void onQueryGamePlaces(ClientId id, void* pData, size_t nDataLen);
	void onQueryGameRooms(ClientId id, void* pData, size_t nDataLen);

	void onQueryPlacesEnd(ClientId id, void* pData, size_t nDataLen);
	void onQueryRoomsEnd(ClientId id, void* pData, size_t nDataLen);

	DECLARE_NETMSG_TABLE()

protected:
	void sendUserInfoMsg(ClientId id, CSUINT16 uSubMsgId, ClientId clientId, CSHANDLE clientHandle, const UserInfoSet& userSet);
	void fillGameKindInfo(GameKindMsgInfo& kindInfo, const GameKindSet& kindSet);
	void fillGamePlaceInfo(GamePlaceMsgInfo& placeInfo, const GamePlaceSet& placeSet);
	void fillGameRoomInfo(GameRoomMsgInfo& roomInfo, const GameRoomSet& roomSet);

	CSINT32 ValidateResultToFailReason(int result) const;

protected:
	CDatabase				m_database;
	char					m_szDSN[256];
	char					m_szGuestName[256];
	char					m_szGuestPW[16];		// password
	int						m_nDefHeadIdx;			// default head index
	int						m_nDefGender;			// default gender
	double					m_dInitMoney;
	unsigned int			m_uInitRoomCard;
	IDBServerUIObserver*	m_pUIObserver;
};

#endif // __DB_SERVER_H__

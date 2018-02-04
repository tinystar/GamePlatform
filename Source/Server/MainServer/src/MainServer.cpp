#include "MainServer.h"
#include "MainMsgDefs.h"
#include <process.h>
#include "GateMsgDefs.h"
#include "DBMsgDefs.h"
#include "GameUserDefs.h"

#define ITEM_ID_GATE		1
#define ITEM_ID_DB			2


NetMsgMapEntry MainServer::s_msgMapArray[] = {
	{ MSG_MAINID_USER, MSG_SUBID_ACCOUNT_LOGIN, static_cast<NetMsgHandler>(&MainServer::onAccountLogin) },
	{ MSG_MAINID_USER, MSG_SUBID_QUICK_LOGIN, static_cast<NetMsgHandler>(&MainServer::onQuickLogin) }
};


MainServer::MainServer()
	: m_sGatePort(0)
	, m_sDBPort(0)
	, m_bStopServer(false)
	, m_hSelectThread(NULL)
	, m_sPort(0)
	, m_uMaxUser(0)
	, m_pUIObserver(NULL)
{
	::memset(m_szGateAddr, 0, sizeof(m_szGateAddr));
	::memset(m_szDBAddr, 0, sizeof(m_szDBAddr));

	m_clientToGate.addEventHandler(this);
	m_clientToDB.addEventHandler(this);
}

MainServer::~MainServer()
{
	m_clientToDB.removeEventHandler(this);
	m_clientToGate.removeEventHandler(this);
}

bool MainServer::onInit(const ServerInitConfig& serverConfig)
{
	if (!BaseGameServer::onInit(serverConfig))
		return false;

	m_sPort = serverConfig.tcpConfig.sPort;

	registerMsgHandler(s_msgMapArray, EzCountOf(s_msgMapArray));
	return true;
}

bool MainServer::onUninit()
{
	removeMsgHandler(s_msgMapArray, EzCountOf(s_msgMapArray));
	return BaseGameServer::onUninit();
}

bool MainServer::onStart()
{
	if (!BaseGameServer::onStart())
		return false;

	if (!connectToGate())
		return false;
	if (!connectToDB())
		return false;

	m_bStopServer = false;
	m_hSelectThread = (HANDLE)::_beginthreadex(NULL, 0, clientSelectThread, this, 0, NULL);
	if (NULL == m_hSelectThread)
		return false;

	return true;
}

bool MainServer::onStop()
{
	if (!BaseGameServer::onStop())
		return false;

	m_bStopServer = true;
	::WaitForSingleObject(m_hSelectThread, INFINITE);
	::CloseHandle(m_hSelectThread);
	m_hSelectThread = NULL;

	m_clientToGate.close();
	m_clientToDB.close();
	return true;
}

void MainServer::setGateSvrAddr(const char* pszAddr, unsigned short sPort)
{
	strncpy(m_szGateAddr, pszAddr, 20);
	m_sGatePort = sPort;
}

void MainServer::setDBSvrAddr(const char* pszAddr, unsigned short sPort)
{
	strncpy(m_szDBAddr, pszAddr, 20);
	m_sDBPort = sPort;
}

void MainServer::onSocketConnected(TcpClientSocket* pClientSock)
{
	if (pClientSock == &m_clientToGate)
	{
		MainConnectMsg connMsg;
		connMsg.header.uMainId = MSG_MAINID_MAIN_TO_GATE;
		connMsg.header.uSubId = MSG_SUBID_MAIN_CONNECT;
		connMsg.sPort = m_sPort;
		connMsg.uMaxUser = m_uMaxUser;

		sendMsgToServer(pClientSock, &connMsg, sizeof(connMsg));

		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToGateSuccess();
	}
	else if (pClientSock == &m_clientToDB)
	{
		if (0 == m_gameList.getChildCount())
			sendMsgToServer(pClientSock, MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEKINDS);

		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToDBSuccess();
	}
}

void MainServer::onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize)
{
	if (pClientSock == &m_clientToGate)
		queueUserItem(ITEM_ID_GATE, pPackage, nSize);
	else if (pClientSock == &m_clientToDB)
		queueUserItem(ITEM_ID_DB, pPackage, nSize);
}

void MainServer::onSocketClosed(TcpClientSocket* pClientSock, int nErrCode)
{
	if (pClientSock == &m_clientToGate)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToGateClosed();

		if (!m_bStopServer && nErrCode != SEC_SUCCESS && nErrCode != SEC_CLOSEDBYPEER && nErrCode != SEC_CONNRESET)
			connectToGate();
	}
	else if (pClientSock == &m_clientToDB)
	{
		notifyClientWhenDBClosed();

		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToDBClosed();

		if (!m_bStopServer && nErrCode != SEC_SUCCESS && nErrCode != SEC_CLOSEDBYPEER && nErrCode != SEC_CONNRESET)
			connectToDB();
	}
}

void MainServer::onTcpClientCloseMsg(ClientId id)
{
	if (id.getUserData() != NULL)		// user already login
	{
		removeLoginUser(id);
	}
	else
	{
		removeClientFromDBReqQueue(id);
	}
}

void MainServer::onTimerMsg(EzUInt uTimerId)
{
	BaseGameServer::onTimerMsg(uTimerId);
}

void MainServer::onUserItemMsg(int itemId, void* pData, size_t nSize)
{
	if (ITEM_ID_GATE == itemId)
		onGateServerMsg(pData, nSize);
	else if (ITEM_ID_DB == itemId)
		onDBServerMsg(pData, nSize);
}

unsigned __stdcall MainServer::clientSelectThread(void* pParam)
{
	MainServer* pMainSvr = (MainServer*)pParam;

	while (!pMainSvr->m_bStopServer)
	{
		TcpClientSocket::select(5);
	}

	return 0;
}

EzULong MainServer::genDBRequestStamp()
{
	static EzULong _s_ulStampSeed = 0;
	return ++_s_ulStampSeed;
}

bool MainServer::connectToGate()
{
	if (m_clientToGate.create() != SEC_SUCCESS)
		return false;
	int nCode = m_clientToGate.connect(m_szGateAddr, m_sGatePort);
	if (nCode != SEC_SUCCESS)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToGateFail(nCode);
		return false;
	}
	return true;
}

bool MainServer::connectToDB()
{
	if (m_clientToDB.create() != SEC_SUCCESS)
		return false;
	int nCode = m_clientToDB.connect(m_szDBAddr, m_sDBPort);
	if (nCode != SEC_SUCCESS)
	{
		if (m_pUIObserver != NULL)
			m_pUIObserver->onUIConnToDBFail(nCode);
		return false;
	}
	return true;
}

void MainServer::onGateServerMsg(void* pData, size_t nSize)
{

}

void MainServer::onDBServerMsg(void* pData, size_t nSize)
{
	EzAssert(pData != NULL && nSize > 0);
	GameMsgHeader* pHeader = (GameMsgHeader*)pData;
	CSUINT16 uMainId = pHeader->uMainId;
	CSUINT16 uSubId = pHeader->uSubId;

	if (MSG_MAINID_DB == uMainId)
	{
		switch (uSubId)
		{
		case MSG_SUBID_DB_LOGIN_SUCCESS:
			onDBLoginSuccess(pData, nSize);
			break;
		case MSG_SUBID_DB_LOGIN_FAILURE:
			onDBLoginFailure(pData, nSize);
			break;
		case MSG_SUBID_GET_GAMEINFO_FAIL:
			onDBQueryGameInfoFail(pData, nSize);
			break;
		case MSG_SUBID_GET_GAMEKIND_SUCC:
			onDBQueryGameKinds(pData, nSize);
			break;
		case MSG_SUBID_GET_GAMEPLACE_SUCC:
			onDBQueryGamePlaces(pData, nSize);
			break;
		case MSG_SUBID_GET_GAMEROOM_SUCC:
			onDBQueryGameRooms(pData, nSize);
			break;
		default:
			EzAssert(false);
			break;
		}
	}
}

bool MainServer::sendMsgToServer(TcpClientSocket* pClientSock, void* pData, size_t nDataLen)
{
	if (NULL == pClientSock || NULL == pData || 0 == nDataLen)
		return false;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)pData, nDataLen);

	pClientSock->sendData(pData, nDataLen);
	return true;
}

bool MainServer::sendMsgToServer(TcpClientSocket* pClientSock, CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved /*= 0*/)
{
	GameMsgHeader header;
	header.uMainId = uMainId;
	header.uSubId = uSubId;
	header.uReserved = uReserved;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)&header, sizeof(header));

	pClientSock->sendData(&header, sizeof(header));
	return true;
}

void MainServer::onAccountLogin(ClientId id, void* pData, size_t nDataLen)
{
	EzAssert(sizeof(AccountLoginMsg) == nDataLen);
	AccountLoginMsg* pAccountLoginMsg = (AccountLoginMsg*)pData;

	ValidateAcctLoginMsg validateMsg;

	ClientStamp stampInfo;
	stampInfo.clientId = id;
	stampInfo.ulStamp = genDBRequestStamp();

	validateMsg.header.uMainId = MSG_MAINID_DB;
	validateMsg.header.uSubId = MSG_SUBID_VALIDATE_ACCOUNT_LOGIN;
	validateMsg.clientStamp = stampInfo;
	memcpy(validateMsg.szAccount, pAccountLoginMsg->szAccount, sizeof(validateMsg.szAccount));
	memcpy(validateMsg.szPassword, pAccountLoginMsg->szPassword, sizeof(validateMsg.szPassword));

	if (sendMsgToServer(&m_clientToDB, &validateMsg, sizeof(validateMsg)))
		m_reqToDBClientQueue.push_back(stampInfo);
}

void MainServer::onQuickLogin(ClientId id, void* pData, size_t nDataLen)
{
	// 消息发送到DB服务器，在DB服务器处理过程中，id对应的客户端可能断开连接，由于底层id结构会重用，
	// 所以在DB服务器处理完返回时，id对于的客户端可能发生了变化，不是之前请求DB服务器时的客户端，
	// 所以不能只用ClientId来唯一确定一个客户端连接，增加ulStamp字段来唯一的标志每次请求以解决这个问题。
	// 此问题只有在登录时会出现，登录成功以后id对应的客户端会被分配一个唯一的玩家Id，不会出现这种情况
	ClientStamp stampInfo;
	stampInfo.clientId = id;
	stampInfo.ulStamp = genDBRequestStamp();

	ClientStampMsg stampMsg;
	stampMsg.header.uMainId = MSG_MAINID_DB;
	stampMsg.header.uSubId = MSG_SUBID_CREATE_GUEST_ACCT;
	stampMsg.clientStamp = stampInfo;

	if (sendMsgToServer(&m_clientToDB, &stampMsg, sizeof(stampMsg)))
		m_reqToDBClientQueue.push_back(stampInfo);
}

void MainServer::onDBLoginSuccess(void* pData, size_t nSize)
{
	EzAssert(sizeof(UserInfoWithClientMsg) == nSize);
	UserInfoWithClientMsg* pUserMsg = (UserInfoWithClientMsg*)pData;

	ClientStamp headStamp = m_reqToDBClientQueue.front();
	if (headStamp == pUserMsg->clientStamp)
	{
		m_reqToDBClientQueue.pop_front();

		UserInfoMsg userMsg;
		userMsg.header.uMainId = MSG_MAINID_USER;
		userMsg.header.uSubId = MSG_SUBID_LOGIN_SUCCESS;
		userMsg.userInfo = pUserMsg->userInfo;

		sendMsg(headStamp.clientId, &userMsg, sizeof(userMsg));

		bool bAdded = addLoginUser(headStamp.clientId, pUserMsg->userInfo);
		if (!EzVerify(bAdded))
			EzLogInfo(_T("Add Login User Failed!\n"));
	}
}

void MainServer::onDBLoginFailure(void* pData, size_t nSize)
{
	EzAssert(sizeof(DBAcctLoginFailMsg) == nSize);
	DBAcctLoginFailMsg* pLoginFailMsg = (DBAcctLoginFailMsg*)pData;

	ClientStamp headStamp = m_reqToDBClientQueue.front();
	if (headStamp == pLoginFailMsg->clientStamp)
	{
		m_reqToDBClientQueue.pop_front();

		LoginFailMsg failMsg;
		failMsg.header.uMainId = MSG_MAINID_USER;
		failMsg.header.uSubId = MSG_SUBID_LOGIN_FAILURE;
		failMsg.nFailReason = pLoginFailMsg->nFailReason;
		sendMsg(headStamp.clientId, &failMsg, sizeof(failMsg));
	}
}

bool MainServer::addLoginUser(ClientId cId, const UserInfo& userInfo)
{
	if (!EzVerify(cId.isValid() && userInfo.userId > 0))
		return false;

	GameUser* pGameUser = new GameUser(userInfo);
	if (NULL == pGameUser)
		return false;

	EzAssert(NULL == cId.getUserData());
	cId.setUserData(pGameUser);

	EzAssert(m_userIdToClientMap.find(pGameUser->getUserId()) == m_userIdToClientMap.end());
	m_userIdToClientMap.insert(UserId2ClientIdMap::value_type(pGameUser->getUserId(), cId));
	return true;
}

bool MainServer::removeLoginUser(ClientId cId)
{
	if (cId.isNull())
		return false;

	void* pUserData = cId.getUserData();
	if (NULL == pUserData)
		return false;

	GameUser* pGameUser = (GameUser*)pUserData;
	
	cId.setUserData(NULL);
	UserId2ClientIdMap::iterator iter = m_userIdToClientMap.find(pGameUser->getUserId());
	EzAssert(iter != m_userIdToClientMap.end());
	if (iter != m_userIdToClientMap.end())
		m_userIdToClientMap.erase(iter);

	delete pGameUser;
	return true;
}

ClientId MainServer::findClientByUserId(EzUInt32 userId) const
{
	UserId2ClientIdMap::const_iterator iter = m_userIdToClientMap.find(userId);
	if (iter == m_userIdToClientMap.end())
		return ClientId::kNull;

	return iter->second;
}

bool MainServer::removeClientFromDBReqQueue(ClientId id)
{
	ClientStampQueue::iterator iter = m_reqToDBClientQueue.begin();
	while (iter != m_reqToDBClientQueue.end())
	{
		if (id == iter->clientId)
		{
			m_reqToDBClientQueue.erase(iter);
			return true;
		}

		++iter;
	}

	return false;
}

void MainServer::notifyClientWhenDBClosed()
{
	ClientStampQueue::iterator iter = m_reqToDBClientQueue.begin();
	while (iter != m_reqToDBClientQueue.end())
	{
		LoginFailMsg failMsg;
		failMsg.header.uMainId = MSG_MAINID_USER;
		failMsg.header.uSubId = MSG_SUBID_LOGIN_FAILURE;
		failMsg.nFailReason = eDBServerClosed;
		sendMsg(iter->clientId, &failMsg, sizeof(failMsg));

		++iter;
	}
	m_reqToDBClientQueue.clear();
}

void MainServer::onDBQueryGameKinds(void* pData, size_t nSize)
{
	GameKindListMsg* pKindListMsg = (GameKindListMsg*)pData;

	for (CSUINT32 i = 0; i < pKindListMsg->uCount; ++i)
	{
		GameKind* pKind = new GameKind();
		pKind->m_kindInfo.initWithGameKindInfo(pKindListMsg->kinds[i]);
		m_gameList.addChild(pKind);

		QueryGamePlaceMsg queryPlaceMsg;
		queryPlaceMsg.header.uMainId = MSG_MAINID_DB;
		queryPlaceMsg.header.uSubId = MSG_SUBID_QUERY_GAMEPLACES;
		queryPlaceMsg.nKindId = pKindListMsg->kinds[i].nKindId;
		sendMsgToServer(&m_clientToDB, &queryPlaceMsg, sizeof(queryPlaceMsg));
	}
}

void MainServer::onDBQueryGamePlaces(void* pData, size_t nSize)
{
	GamePlaceListMsg* pPlaceListMsg = (GamePlaceListMsg*)pData;

	for (CSUINT32 i = 0; i < pPlaceListMsg->uCount; ++i)
	{
		GamePlace* pPlace = new GamePlace();
		pPlace->m_placeInfo.initWithGamePlaceInfo(pPlaceListMsg->places[i]);
		GameNode* pKind = m_gameList.findChildById(pPlaceListMsg->places[i].nKindId);
		if (EzVerify(pKind != NULL))
			pKind->addChild(pPlace);

		QueryGameRoomMsg queryRoomMsg;
		queryRoomMsg.header.uMainId = MSG_MAINID_DB;
		queryRoomMsg.header.uSubId = MSG_SUBID_QUERY_GAMEROOMS;
		queryRoomMsg.nKindId = pPlaceListMsg->places[i].nKindId;
		queryRoomMsg.nPlaceId = pPlaceListMsg->places[i].nPlaceId;
		sendMsgToServer(&m_clientToDB, &queryRoomMsg, sizeof(queryRoomMsg));
	}
}

void MainServer::onDBQueryGameRooms(void* pData, size_t nSize)
{
	GameRoomListMsg* pRoomListMsg = (GameRoomListMsg*)pData;

	for (CSUINT32 i = 0; i < pRoomListMsg->uCount; ++i)
	{
		GameRoom* pRoom = new GameRoom();
		pRoom->m_roomInfo.initWithGameRoomInfo(pRoomListMsg->rooms[i]);
		GameNode* pKind = m_gameList.findChildById(pRoomListMsg->rooms[i].nKindId);
		if (EzVerify(pKind != NULL))
		{
			GameNode* pPlace = pKind->findChildById(pRoomListMsg->rooms[i].nPlaceId);
			if (EzVerify(pPlace != NULL))
				pPlace->addChild(pRoom);
		}
	}
}

void MainServer::onDBQueryGameInfoFail(void* pData, size_t nSize)
{
	EzAssert(sizeof(GetGameInfoFailMsg) == nSize);
	GetGameInfoFailMsg* pGetFailMsg = (GetGameInfoFailMsg*)pData;

	if (m_pUIObserver != NULL)
		m_pUIObserver->onUIGetGameInfoFail(pGetFailMsg->nInfoType, pGetFailMsg->szDetail);
}

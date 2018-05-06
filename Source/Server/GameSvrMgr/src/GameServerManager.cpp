#include "GameServerManager.h"
#include "DBMsgDefs.h"
#include "GameNodeData.h"
#include "GameRoomServer.h"
#include "GameModuleDef.h"


GameServerManager* GameServerManager::m_pSharedManager = NULL;

GameServerManager* GameServerManager::getInstance()
{
	if (NULL == m_pSharedManager)
		m_pSharedManager = new GameServerManager();

	return m_pSharedManager;
}

void GameServerManager::destroyInstance()
{
	EZ_SAFE_DELETE(m_pSharedManager);
}

GameServerManager::GameServerManager()
	: m_pEventListener(NULL)
{
	m_clientToDB.addEventHandler(this);
}

GameServerManager::~GameServerManager()
{
	m_clientToDB.removeEventHandler(this);
}

void GameServerManager::updateGameList()
{
	if (!m_gameList.isEmpty())
		m_gameList.setEmpty();

	sendMsgToServer(&m_clientToDB, MSG_MAINID_DB, MSG_SUBID_QUERY_GAMEKINDS);
}

bool GameServerManager::loadGameModule(GameKind* pGameKind)
{
	if (NULL == pGameKind)
		return false;

	void* pData = pGameKind->getUserData();
	if (NULL == pData)
	{
		GameModuleMgr* pMgr = new GameModuleMgr();
		if (NULL == pMgr)
			return false;

		pGameKind->setUserData(pMgr);
		pData = pMgr;
	}

	GameModuleMgr* pModuleMgr = (GameModuleMgr*)pData;
	if (pModuleMgr->isLoaded())
		return true;

	if (pGameKind->m_kindInfo.sSvrModFullPath.empty())
	{
		pGameKind->m_kindInfo.sSvrModFullPath = "Games\\";
		pGameKind->m_kindInfo.sSvrModFullPath += pGameKind->m_kindInfo.szServerModule;
	}

	bool bRet = pModuleMgr->loadGameModule(pGameKind->m_kindInfo.sSvrModFullPath.c_str());
	if (!bRet)
		return false;

	GameAppEntryPointFuncPtr pAppEntryPoint = (GameAppEntryPointFuncPtr)pModuleMgr->getSymbAddress(GAMEAPPENTRYPOINT_PROC_NAME);
	if (!pAppEntryPoint(kInitAppMsg))
	{
		pAppEntryPoint(kUnloadAppMsg);
		pModuleMgr->unloadGameModule();
		return false;
	}

	GetGameAppVersionFuncPtr pGetAppVersion = (GetGameAppVersionFuncPtr)pModuleMgr->getSymbAddress(GETGAMEAPPVERSION_PROC_NAME);
	const char* pVersion = pGetAppVersion();
	// Check the game module version to avoid DLL hell problem.
	if (strcmp(pVersion, GAME_VERSION_STRING) != 0)
	{
		GUITHREADINFO info;
		::memset(&info, 0, sizeof(info));
		info.cbSize = sizeof(GUITHREADINFO);
		::GetGUIThreadInfo(::GetCurrentThreadId(), &info);
		::MessageBox(info.hwndActive, _T("The version isn't match! Please update the sdk and recompile the module."), _T("error"), MB_OK);
		pAppEntryPoint(kUnloadAppMsg);
		pModuleMgr->unloadGameModule();
		return false;
	}

	return true;
}

bool GameServerManager::unloadGameModule(GameKind* pGameKind)
{
	if (NULL == pGameKind)
		return false;

	void* pData = pGameKind->getUserData();
	if (NULL == pData)
		return false;

	GameModuleMgr* pModuleMgr = (GameModuleMgr*)pData;
	if (!pModuleMgr->isLoaded())
		return false;

	// cleanup all room server depend on this module
	GameRoomIterator iter(pGameKind);
	for (iter.start(); !iter.done(); iter.next())
	{
		cleanupGameRoom(iter.curNode());
	}

	GameAppEntryPointFuncPtr pAppEntryPoint = (GameAppEntryPointFuncPtr)pModuleMgr->getSymbAddress(GAMEAPPENTRYPOINT_PROC_NAME);
	pAppEntryPoint(kUnloadAppMsg);

	return pModuleMgr->unloadGameModule();
}

void GameServerManager::cleanupGameModule(GameKind* pGameKind)
{
	if (NULL == pGameKind)
		return;

	void* pData = pGameKind->getUserData();
	if (NULL == pData)
		return;

	unloadGameModule(pGameKind);

	GameModuleMgr* pModuleMgr = (GameModuleMgr*)pData;
	pGameKind->setUserData(NULL);
	delete pModuleMgr;
}

bool GameServerManager::startGameRoom(GameRoom* pGameRoom)
{
	if (pGameRoom != NULL)
	{
		ServerInitConfig svrConfig;
		svrConfig.tcpConfig.sPort = pGameRoom->m_roomInfo.sServerPort;
		svrConfig.tcpConfig.nMaxPackageSize = m_config.nRoomPkgSize;

		void* pData = pGameRoom->getUserData();
		if (pData != NULL)
		{
			GameRoomMgr* pRoomMgr = (GameRoomMgr*)pData;
			if (pRoomMgr->isRunning())
				return true;

			return pRoomMgr->startRoomServer(svrConfig);
		}

		GameNode* pParent = pGameRoom->getParent();
		if (pParent != NULL && GameNode::kTypePlace == pParent->type())
		{
			GamePlace* pPlace = (GamePlace*)pParent;
			pParent = pPlace->getParent();
			if (pParent != NULL && GameNode::kTypeKind == pParent->type())
			{
				GameKind* pKind = (GameKind*)pParent;
				void* pKindData = pKind->getUserData();
				if (NULL == pKindData)
					return false;
				GameModuleMgr* pModuleMgr = (GameModuleMgr*)pKindData;
				if (!pModuleMgr->isLoaded())	// Game module has not loaded yet!
					return false;

				GetGameDeskFactoryFuncPtr pGetGameDeskFactory = (GetGameDeskFactoryFuncPtr)pModuleMgr->getSymbAddress(GETGAMEDESKFACTORY_PROC_NAME);
				GameDeskFactory* pDeskFactory = pGetGameDeskFactory();
				if (NULL == pDeskFactory)
					return false;

				RoomContext roomCtx;
				roomCtx.placeType = pPlace->m_placeInfo.placeType;
				roomCtx.dEnterLimit = pPlace->m_placeInfo.dEnterLimit;
				roomCtx.dBasePoint = pPlace->m_placeInfo.dBasePoint;
				roomCtx.uMaxUserCount = pGameRoom->m_roomInfo.uMaxUserCount;
				roomCtx.pDeskFactory = pDeskFactory;

				GameRoomMgr* pRoomMgr = new GameRoomMgr(roomCtx);
				if (NULL == pRoomMgr)
					return false;

				pGameRoom->setUserData(pRoomMgr);
				return pRoomMgr->startRoomServer(svrConfig);
			}
		}
	}

	return false;
}

bool GameServerManager::stopGameRoom(GameRoom* pGameRoom)
{
	if (NULL == pGameRoom)
		return false;

	void* pData = pGameRoom->getUserData();
	if (NULL == pData)
		return false;

	GameRoomMgr* pRoomMgr = (GameRoomMgr*)pData;
	if (!pRoomMgr->isRunning())
		return false;

	return pRoomMgr->stopRoomServer();
}

void GameServerManager::cleanupGameRoom(GameRoom* pGameRoom)
{
	if (NULL == pGameRoom)
		return;

	void* pData = pGameRoom->getUserData();
	if (NULL == pData)
		return;

	stopGameRoom(pGameRoom);

	GameRoomMgr* pRoomMgr = (GameRoomMgr*)pData;
	pGameRoom->setUserData(NULL);
	delete pRoomMgr;
}

void GameServerManager::onSocketConnected(TcpClientSocket* pClientSock)
{

}

void GameServerManager::onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize)
{
	GameMsgHeader* pHeader = (GameMsgHeader*)pPackage;
	CSUINT16 uMainId = pHeader->uMainId;
	CSUINT16 uSubId = pHeader->uSubId;

	if (MSG_MAINID_DB == uMainId)
	{
		switch (uSubId)
		{
		case MSG_SUBID_GET_GAMEKIND_SUCC:
			onDBQueryGameKinds(pPackage, nSize);
			break;
		case MSG_SUBID_GET_GAMEPLACE_SUCC:
			onDBQueryGamePlaces(pPackage, nSize);
			break;
		case MSG_SUBID_GET_GAMEROOM_SUCC:
			onDBQueryGameRooms(pPackage, nSize);
			break;
		case MSG_SUBID_GET_GAMEINFO_FAIL:
			onDBQueryGameInfoFail(pPackage, nSize);
			break;
		case MSG_SUBID_QUERY_PLACES_END:
			onDBQueryPlacesEnd(pPackage, nSize);
			break;
		case MSG_SUBID_QUERY_ROOMS_END:
			onDBQueryRoomsEnd(pPackage, nSize);
			break;
		default:
			EzAssert(false);
			break;
		}
	}
}

void GameServerManager::onSocketClosed(TcpClientSocket* pClientSock, int nErrCode)
{
	if (nErrCode != SEC_SUCCESS && nErrCode != SEC_CLOSEDBYPEER && nErrCode != SEC_CONNRESET)
		connectToDB();
}

bool GameServerManager::initialize(const MgrInitConfig& config)
{
	m_config = config;

	return connectToDB();
}

bool GameServerManager::unInitialize()
{
	m_clientToDB.close();

	return true;
}

bool GameServerManager::connectToDB()
{
	if (m_clientToDB.create() != SEC_SUCCESS)
		return false;
	int nCode = m_clientToDB.connect(m_config.szDBAddress, m_config.sDBPort);
	if (nCode != SEC_SUCCESS)
		return false;

	return true;
}

bool GameServerManager::sendMsgToServer(TcpClientSocket* pClientSock, void* pData, size_t nDataLen)
{
	if (NULL == pClientSock || NULL == pData || 0 == nDataLen)
		return false;

	pClientSock->sendData(pData, nDataLen);
	return true;
}

bool GameServerManager::sendMsgToServer(TcpClientSocket* pClientSock, CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved /*= 0*/)
{
	GameMsgHeader header;
	header.uMainId = uMainId;
	header.uSubId = uSubId;
	header.uReserved = uReserved;

	pClientSock->sendData(&header, sizeof(header));
	return true;
}

void GameServerManager::doUpdate()
{
	TcpClientSocket::select(1);
}

void GameServerManager::onDBQueryGameInfoFail(void* pData, size_t nSize)
{
	EzAssert(sizeof(GetGameInfoFailMsg) == nSize);
	GetGameInfoFailMsg* pGetFailMsg = (GetGameInfoFailMsg*)pData;

	// Todo
}

void GameServerManager::onDBQueryGameKinds(void* pData, size_t nSize)
{
	GameKindListMsg* pKindListMsg = (GameKindListMsg*)pData;

	for (CSUINT32 i = 0; i < pKindListMsg->uCount; ++i)
	{
		GameKind* pKind = new GameKind();
		pKind->m_kindInfo.initWithGameKindMsg(pKindListMsg->kinds[i]);
		m_gameList.addGameKind(pKind);

		QueryGamePlaceMsg queryPlaceMsg;
		queryPlaceMsg.header.uMainId = MSG_MAINID_DB;
		queryPlaceMsg.header.uSubId = MSG_SUBID_QUERY_GAMEPLACES;
		queryPlaceMsg.nKindId = pKindListMsg->kinds[i].nKindId;
		sendMsgToServer(&m_clientToDB, &queryPlaceMsg, sizeof(queryPlaceMsg));
	}

	// 获取游戏列表时采用的是依次请求游戏类型、游戏场次、游戏房间的方式，这带来一个问题就是无法直接知道
	// 游戏列表获取完毕，在此采用一个简单的办法，利用Tcp协议有顺序的特点，在发送完请求游戏场次的消息后再
	// 多发送一个消息，DB服务器回应相同的消息，等收到回复消息时场次已经完全收到并发送完所有请求房间的消
	// 息，此时再发送一个消息，等收到消息时说明房间已经获取完毕。
	sendMsgToServer(&m_clientToDB, MSG_MAINID_DB, MSG_SUBID_QUERY_PLACES_END);
}

void GameServerManager::onDBQueryGamePlaces(void* pData, size_t nSize)
{
	GamePlaceListMsg* pPlaceListMsg = (GamePlaceListMsg*)pData;

	for (CSUINT32 i = 0; i < pPlaceListMsg->uCount; ++i)
	{
		GamePlace* pPlace = new GamePlace();
		pPlace->m_placeInfo.initWithGamePlaceMsg(pPlaceListMsg->places[i]);		
		m_gameList.addGamePlace(pPlaceListMsg->places[i].nKindId, pPlace);		

		QueryGameRoomMsg queryRoomMsg;
		queryRoomMsg.header.uMainId = MSG_MAINID_DB;
		queryRoomMsg.header.uSubId = MSG_SUBID_QUERY_GAMEROOMS;
		queryRoomMsg.nKindId = pPlaceListMsg->places[i].nKindId;
		queryRoomMsg.nPlaceId = pPlaceListMsg->places[i].nPlaceId;
		sendMsgToServer(&m_clientToDB, &queryRoomMsg, sizeof(queryRoomMsg));
	}
}

void GameServerManager::onDBQueryGameRooms(void* pData, size_t nSize)
{
	GameRoomListMsg* pRoomListMsg = (GameRoomListMsg*)pData;

	for (CSUINT32 i = 0; i < pRoomListMsg->uCount; ++i)
	{
		GameRoom* pRoom = new GameRoom();
		pRoom->m_roomInfo.initWithGameRoomMsg(pRoomListMsg->rooms[i]);
		m_gameList.addGameRoom(pRoomListMsg->rooms[i].nKindId, pRoomListMsg->rooms[i].nPlaceId, pRoom);
	}
}

void GameServerManager::onDBQueryPlacesEnd(void* pData, size_t nSize)
{
	sendMsgToServer(&m_clientToDB, MSG_MAINID_DB, MSG_SUBID_QUERY_ROOMS_END);
}

void GameServerManager::onDBQueryRoomsEnd(void* pData, size_t nSize)
{
	if (m_pEventListener != NULL)
		m_pEventListener->onUpdateGameListOver();
}

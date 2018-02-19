#include "GameServerManager.h"
#include "DBMsgDefs.h"


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
	return false;
}

bool GameServerManager::unloadGameModule(GameKind* pGameKind)
{
	return false;
}

bool GameServerManager::startGameRoom(GameRoom* pGameRoom)
{
	return false;
}

bool GameServerManager::stopGameRoom(GameRoom* pGameRoom)
{
	return false;
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

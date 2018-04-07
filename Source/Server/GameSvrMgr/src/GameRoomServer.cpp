#include "GameRoomServer.h"
#include "GameDesk.h"


GameRoomServer::GameRoomServer(const RoomContext& ctx)
	: m_RoomCtx(ctx)
	, m_pGameDesks(NULL)
	, m_nDeskCount(0)
{
	if (!EzVerify(m_RoomCtx.pDeskFactory != NULL))
		throw EzException(_T("GameDeskFactory function pointer can't be NULL!"), _T(__FILE__), __LINE__);
}

GameRoomServer::~GameRoomServer()
{
	cleanGameDesks();
}

bool GameRoomServer::onInit(const ServerInitConfig& serverConfig)
{
	if (!BaseGameServer::onInit(serverConfig))
		return false;
	
	return initGameDesks();
}

bool GameRoomServer::onUninit()
{
	cleanGameDesks();
	return BaseGameServer::onUninit();
}

bool GameRoomServer::onStart()
{
	return BaseGameServer::onStart();
}

bool GameRoomServer::onStop()
{
	return BaseGameServer::onStop();
}

bool GameRoomServer::initGameDesks()
{
	// Create a temp desk to obtain some informations.
	GameDesk* pTmpDesk = m_RoomCtx.pDeskFactory->createGameDesk();
	if (NULL == pTmpDesk)
		return false;

	int nDeskChairs = pTmpDesk->getChairCount();
	m_nDeskCount = (int)m_RoomCtx.uMaxUserCount / nDeskChairs + 1;

	m_RoomCtx.pDeskFactory->destroyGameDesk(pTmpDesk);

	m_pGameDesks = new GameDesk*[m_nDeskCount];
	if (NULL == m_pGameDesks)
		return false;
	
	for (int i = 0; i < m_nDeskCount; ++i)
	{
		m_pGameDesks[i] = m_RoomCtx.pDeskFactory->createGameDesk();
		if (NULL == m_pGameDesks[i])
		{
			EzLogError(_T("Create game desk failed, memory may not enough!\n"));
			return false;
		}
	}

	return true;
}

void GameRoomServer::cleanGameDesks()
{
	if (m_pGameDesks != NULL)
	{
		for (int i = 0; i < m_nDeskCount; ++i)
			m_RoomCtx.pDeskFactory->destroyGameDesk(m_pGameDesks[i]);
		EZ_SAFE_DELETE_ARRAY(m_pGameDesks);
	}

	m_nDeskCount = 0;
}

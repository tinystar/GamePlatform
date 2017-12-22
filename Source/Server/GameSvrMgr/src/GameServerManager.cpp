#include "GameServerManager.h"


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

}

GameServerManager::~GameServerManager()
{

}

bool GameServerManager::updateGameList()
{
	return false;
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

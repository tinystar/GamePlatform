#include "GameNodeData.h"
#include "GameModuleDef.h"
#include "GameRoomServer.h"


//------------------------------------------------------
// GameModuleMgr
//------------------------------------------------------
GameModuleMgr::GameModuleMgr()
	: m_hGameModule(NULL)
{

}

GameModuleMgr::~GameModuleMgr()
{
	unloadGameModule();
}

bool GameModuleMgr::loadGameModule(const char* pszModName)
{
	if (NULL == pszModName || 0 == *pszModName)
		return false;

	m_hGameModule = EzSys::loadLibrary(pszModName);
	if (m_hGameModule != NULL)
	{
		if (NULL == EzSys::getSymbolAddress(m_hGameModule, GAMEAPPENTRYPOINT_PROC_NAME) ||
			NULL == EzSys::getSymbolAddress(m_hGameModule, GETGAMEDESKFACTORY_PROC_NAME) ||
			NULL == EzSys::getSymbolAddress(m_hGameModule, GETGAMEAPPVERSION_PROC_NAME))
		{
			unloadGameModule();
			return false;
		}
	}

	return m_hGameModule != NULL;
}

bool GameModuleMgr::unloadGameModule()
{
	if (NULL == m_hGameModule)
		return false;

	EzSys::freeLibrary(m_hGameModule);
	m_hGameModule = NULL;
	return true;
}

void* GameModuleMgr::getSymbAddress(const char* pszSymbName) const
{
	if (NULL == m_hGameModule)
		return NULL;

	return EzSys::getSymbolAddress(m_hGameModule, pszSymbName);
}


//------------------------------------------------------
// GameRoomMgr
//------------------------------------------------------
GameRoomMgr::GameRoomMgr(const RoomContext& ctx)
	: m_pRoomServer(NULL)
	, m_bRunning(false)
{
	bool bRet = createRoomServer(ctx);
	if (!EzVerify(bRet))
		throw EzException(_T("Memory not enough!"), _T(__FILE__), __LINE__);
}

GameRoomMgr::~GameRoomMgr()
{
	destroyRoomServer();
}

bool GameRoomMgr::startRoomServer(const ServerInitConfig& config)
{
	EzAssert(m_pRoomServer != NULL);
	SVCErrorCode ec = m_pRoomServer->init(config);
	if (ec != eOk)
		return false;

	ec = m_pRoomServer->start();
	if (ec != eOk)
		return false;

	m_bRunning = true;
	return true;
}

bool GameRoomMgr::stopRoomServer()
{
	if (!m_bRunning)
		return true;

	m_pRoomServer->stop();
	m_pRoomServer->unInit();
	m_bRunning = false;
	return true;
}

bool GameRoomMgr::createRoomServer(const RoomContext& ctx)
{
	EzAssert(m_pRoomServer == NULL);
	m_pRoomServer = new GameRoomServer(ctx);
	return m_pRoomServer != NULL;
}

bool GameRoomMgr::destroyRoomServer()
{
	delete m_pRoomServer;
	m_pRoomServer = NULL;
	return true;
}

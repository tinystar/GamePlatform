#include "GameRoomServer.h"
#include "GameDesk.h"
#include "DeskTimersManager.h"

#define IDT_UPDATE_DESK_TIMER_MGR		1


GameRoomServer::GameRoomServer(const RoomContext& ctx)
	: m_RoomCtx(ctx)
	, m_pGameDesks(NULL)
	, m_nDeskCount(0)
{
	if (!EzVerify(m_RoomCtx.pDeskFactory != NULL))
		throw EzException(_T("GameDeskFactory function pointer can't be NULL!"), _T(__FILE__), __LINE__);

	m_pDeskTimersMgr = new DeskTimersManager();
	if (!EzVerify(m_pDeskTimersMgr != NULL))
		throw EzException(_T("Allocate DeskTimersManager failed!"), _T(__FILE__), __LINE__);
}

GameRoomServer::~GameRoomServer()
{
	cleanGameDesks();
	EZ_SAFE_DELETE(m_pDeskTimersMgr);
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
	if (!BaseGameServer::onStart())
		return false;

	m_pDeskTimersMgr->clear();
	setTimer(IDT_UPDATE_DESK_TIMER_MGR, m_pDeskTimersMgr->updateInterval());

	return true;
}

bool GameRoomServer::onStop()
{
	killTimer(IDT_UPDATE_DESK_TIMER_MGR);
	return BaseGameServer::onStop();
}

bool GameRoomServer::initGameDesks()
{
	// Create a temp desk to obtain some informations about the desk.
	GameDesk* pTmpDesk = m_RoomCtx.pDeskFactory->createGameDesk();
	if (NULL == pTmpDesk)
		return false;

	int nDeskChairs = pTmpDesk->getChairCount();
	m_nDeskCount = ((int)m_RoomCtx.uMaxUserCount + nDeskChairs - 1) / nDeskChairs;

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

		m_pGameDesks[i]->init(this, i);
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

bool GameRoomServer::setDeskTimer(GameDesk* pDesk, EzUInt uTimerId, EzUInt uElapse)
{
	return m_pDeskTimersMgr->setDeskTimer(pDesk, uTimerId, uElapse);
}

bool GameRoomServer::killDeskTimer(GameDesk* pDesk, EzUInt uTimerId)
{
	return m_pDeskTimersMgr->killDeskTimer(pDesk, uTimerId);
}

void GameRoomServer::onTimerMsg(EzUInt uTimerId)
{
	switch (uTimerId)
	{
	case IDT_UPDATE_DESK_TIMER_MGR:
		m_pDeskTimersMgr->update();
		break;
	default:
		break;
	}
}

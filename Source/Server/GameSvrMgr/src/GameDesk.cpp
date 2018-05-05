#include "GameDesk.h"
#include "GameRoomServer.h"


GameDesk::GameDesk(int nChairs)
	: m_pRoomServer(NULL)
	, m_nChairCount(nChairs)
	, m_nDeskNumber(INVALID_DESK_NUMBER)
{

}

GameDesk::~GameDesk()
{

}

bool GameDesk::init(GameRoomServer* pRoomServer, int nNumber)
{
	if (!EzVerify(pRoomServer != NULL && nNumber >= 0))
		return false;

	m_pRoomServer = pRoomServer;
	m_nDeskNumber = nNumber;
	return true;
}

bool GameDesk::setTimer(EzUInt uTimerId, EzUInt uElapse)
{
	return m_pRoomServer->setDeskTimer(this, uTimerId, uElapse);
}

bool GameDesk::killTimer(EzUInt uTimerId)
{
	return m_pRoomServer->killDeskTimer(this, uTimerId);
}

void GameDesk::onTimer(EzUInt uTimerId)
{
}

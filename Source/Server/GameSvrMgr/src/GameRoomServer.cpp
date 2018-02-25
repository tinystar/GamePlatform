#include "GameRoomServer.h"


GameRoomServer::GameRoomServer(const RoomContext& ctx)
	: m_pGameDesks(NULL)
	, m_RoomCtx(ctx)
{
	if (!EzVerify(m_RoomCtx.pDeskFactory != NULL))
		throw EzException(_T("GameDeskFactory function pointer can't be NULL!"), _T(__FILE__), __LINE__);
}

GameRoomServer::~GameRoomServer()
{

}

bool GameRoomServer::onInit(const ServerInitConfig& serverConfig)
{
	return BaseGameServer::onInit(serverConfig);
}

bool GameRoomServer::onUninit()
{
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

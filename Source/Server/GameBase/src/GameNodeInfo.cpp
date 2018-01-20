#include "GameNodeInfo.h"


void GameKindInfo::initWithGameKindInfo(const GameKindMsgInfo& kindInfo)
{
	nKindId = kindInfo.nKindId;
	strcpy(szGameName, kindInfo.szGameName);
	strcpy(szClientModule, kindInfo.szCliModInfo);
	strcpy(szServerModule, kindInfo.szSvrModInfo);
	strcpy(szVersion, kindInfo.szVersion);
}

void GamePlaceInfo::initWithGamePlaceInfo(const GamePlaceMsgInfo& placeInfo)
{
	nPlaceId = placeInfo.nPlaceId;
	strcpy(szPlaceName, placeInfo.szPlaceName);
	placeType = (PlaceType)placeInfo.nPlaceType;
	dEnterLimit = placeInfo.uEnterLimit / 100.0;
	dBasePoint = placeInfo.uBasePoint / 100.0;
}

void GameRoomInfo::initWithGameRoomInfo(const GameRoomMsgInfo& roomInfo)
{
	nRoomId = roomInfo.nRoomId;
	strcpy(szRoomName, roomInfo.szRoomName);
	strcpy(szServerIp, roomInfo.szServerIp);
	sServerPort = roomInfo.sServerPort;
}

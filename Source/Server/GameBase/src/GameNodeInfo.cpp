#include "GameNodeInfo.h"


void GameKindInfo::initWithGameKindMsg(const GameKindMsgInfo& kindInfo)
{
	nKindId = kindInfo.nKindId;
	strcpy(szGameName, kindInfo.szGameName);
	strcpy(szClientModule, kindInfo.szCliModInfo);
	strcpy(szServerModule, kindInfo.szSvrModInfo);
	strcpy(szVersion, kindInfo.szVersion);
}

void GameKindInfo::setGameKindMsg(GameKindMsgInfo& kindInfo)
{
	kindInfo.nKindId = nKindId;
	strcpy(kindInfo.szGameName, szGameName);
	strcpy(kindInfo.szCliModInfo, szClientModule);
	strcpy(kindInfo.szSvrModInfo, szServerModule);
	strcpy(kindInfo.szVersion, szVersion);
}

void GamePlaceInfo::initWithGamePlaceMsg(const GamePlaceMsgInfo& placeInfo)
{
	nPlaceId = placeInfo.nPlaceId;
	strcpy(szPlaceName, placeInfo.szPlaceName);
	placeType = (PlaceType)placeInfo.nPlaceType;
	dEnterLimit = placeInfo.uEnterLimit / 100.0;
	dBasePoint = placeInfo.uBasePoint / 100.0;
}

void GamePlaceInfo::setGamePlaceMsg(GamePlaceMsgInfo& placeInfo)
{
	placeInfo.nPlaceId = nPlaceId;
	strcpy(placeInfo.szPlaceName, szPlaceName);
	placeInfo.nPlaceType = placeType;
	placeInfo.uEnterLimit = dEnterLimit * 100;
	placeInfo.uBasePoint = dBasePoint * 100;
}

void GameRoomInfo::initWithGameRoomMsg(const GameRoomMsgInfo& roomInfo)
{
	nRoomId = roomInfo.nRoomId;
	strcpy(szRoomName, roomInfo.szRoomName);
	strcpy(szServerIp, roomInfo.szServerIp);
	sServerPort = roomInfo.sServerPort;
}

void GameRoomInfo::setGameRoomMsg(GameRoomMsgInfo& roomInfo)
{
	roomInfo.nRoomId = nRoomId;
	strcpy(roomInfo.szRoomName, szRoomName);
	strcpy(roomInfo.szServerIp, szServerIp);
	roomInfo.sServerPort = sServerPort;
}

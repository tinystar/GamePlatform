-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2018/2/10
-- *********************************************************************

local PromptLayer = import(".PromptLayer")

local PlaceScene = class("PlaceScene", cc.load("mvc").ViewBase)

PlaceScene.RESOURCE_FILENAME = "PlaceScene.csb"
PlaceScene.RESOURCE_BINDING = {
    ["Text_GameName"] = {["varname"] = "GameNameText"},
    ["Button_GoBack"] = {["varname"] = "GoBackBtn"},
    ["ListView_PlaceList"] = {["varname"] = "PlaceListView"}
}

-- -----------------------------------------------------
-- MainMsgId            SubMsgId            MsgHandler
-- -----------------------------------------------------
PlaceScene.MainMsgMap = {
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_ROOM_ADDRESS, MsgHandler = "onRoomAddressMsg"},
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_ENTER_PLACE_FAIL, MsgHandler = "onEnterPlaceFailMsg"}
}


function PlaceScene:onCreate()
    self.GoBackBtn:addClickEventListener(handler(self, self.onGoBackBtnClicked))

    self.PlaceListView:setScrollBarEnabled(false)

    local itemNode = cc.CSLoader:createNode("PlaceItem.csb")
    self.GamePlaceItem = itemNode:getChildByName("Panel_Item")
    self.GamePlaceItem:retain()
end

function PlaceScene:initWithGameKind(kindInfo)
    self.gameKind_ = kindInfo

    self.GameNameText:setString(kindInfo.GameName)

    for i = 1, #kindInfo.PlaceList do
        local placeItem = self:createPlaceItem(kindInfo.PlaceList[i])
        self.PlaceListView:addChild(placeItem)
    end

    self.PlaceListView:requestDoLayout()
end

function PlaceScene:initEventListeners()
    self.eventListeners_ = {}

    local eventDispatcher = self:getEventDispatcher()

    local listener = cc.EventListenerCustom:create(CustEvents.ConnRoomSucc, handler(self, self.onEventRoomServerConnectSuccessful))
    eventDispatcher:addEventListenerWithFixedPriority(listener, 1)
    table.insert(self.eventListeners_, listener)

    listener = cc.EventListenerCustom:create(CustEvents.ConnRoomFail, handler(self, self.onEventRoomServerConnectFailed))
    eventDispatcher:addEventListenerWithFixedPriority(listener, 1)
    table.insert(self.eventListeners_, listener)
end

function PlaceScene:cleanupEventListeners()
    for i, listener in ipairs(self.eventListeners_) do
        self:getEventDispatcher():removeEventListener(listener)
    end

    self.eventListeners_ = nil
end

function PlaceScene:initSocketMsgMap(sockObj, msgMap)
    for _, entry in ipairs(msgMap) do
        SocketMsgMapper.registerAnMapEntry(sockObj, entry.MainMsgId, entry.SubMsgId, handler(self, self[entry.MsgHandler]))
    end
end

function PlaceScene:unInitSocketMsgMap(sockObj, msgMap)
    for _, entry in ipairs(msgMap) do
        SocketMsgMapper.removeAnMapEntry(sockObj, entry.MainMsgId, entry.SubMsgId)
    end
end

function PlaceScene:onEnter()
    self:initEventListeners()
    self:initSocketMsgMap(__GData__.MainSocket, PlaceScene.MainMsgMap)
end

function PlaceScene:onExit()
    self:unInitSocketMsgMap(__GData__.MainSocket, PlaceScene.MainMsgMap)

    self.GamePlaceItem:release()
    self:cleanupEventListeners()
end

function PlaceScene:createPlaceItem(placeInfo)
    local item = self.GamePlaceItem:clone()
    item.GamePlace_ = placeInfo
    item:addClickEventListener(handler(self, self.onGamePlaceClicked))

    local limitText = item:getChildByName("Text_Limit")
    local basePointText = item:getChildByName("Text_BasePoint")
    limitText:setString(placeInfo.EnterLimit)
    basePointText:setString(placeInfo.BasePoint)

    return item
end

function PlaceScene:onGoBackBtnClicked(sender)
    print("------onGoBackBtnClicked------")
    self:getApp():enterScene("LobbyScene")
end

function PlaceScene:onGamePlaceClicked(sender)
    print("------onGamePlaceClicked------")
    local placeInfo = sender.GamePlace_
    if __GData__.GameUser.Money < placeInfo.EnterLimit then
        -- Todo: jump to shop
        return
    end

    __GData__.MainSocket:sendData(packEnterGamePlaceMsg(self.gameKind_.KindId, sender.GamePlace_.PlaceId))
    -- Todo: ??????????????...
end

function PlaceScene:onRoomAddressMsg(sockObj, msg, msgLen)
    print("-------onRoomAddressMsg-------")
    local RoomAddrMsg = parseRoomAddressMsg(msg, msgLen)

    print(RoomAddrMsg.RoomAddr)
    print(RoomAddrMsg.RoomPort)

    __GData__.RoomSvrAddress = RoomAddrMsg.RoomAddr
    __GData__.RoomSvrPort = RoomAddrMsg.RoomPort

    -- connect to room server
    connectRoomServer()
end

function PlaceScene:onEnterPlaceFailMsg(sockObj, msg, msgLen)
    print("-------onEnterPlaceFailMsg-------")
end

function PlaceScene:onEventRoomServerConnectSuccessful(event)

end

function PlaceScene:onEventRoomServerConnectFailed(event)
    local promptLayer = PromptLayer:create(gres.str.CONNECT_ROOM_FAILED .. event._usedata)
    self:addChild(promptLayer)
end

return PlaceScene

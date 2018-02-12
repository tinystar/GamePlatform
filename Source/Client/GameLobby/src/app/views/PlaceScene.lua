-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2018/2/10
-- *********************************************************************

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
    self:initSocketMsgMap(__GData__.MainSocket, PlaceScene.MainMsgMap)
end

function PlaceScene:onExit()
    self:unInitSocketMsgMap(__GData__.MainSocket, PlaceScene.MainMsgMap)

    self.GamePlaceItem:release()
end

function PlaceScene:createPlaceItem(placeInfo)
    local item = self.GamePlaceItem:clone()

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

return PlaceScene

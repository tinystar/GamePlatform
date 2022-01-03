-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2018/1/7
-- *********************************************************************

local LobbyScene = class("LobbyScene", cc.load("mvc").ViewBase)

LobbyScene.RESOURCE_FILENAME = "LobbyScene.csb"
LobbyScene.RESOURCE_BINDING = {
    ["Panel_UserInfo"] = {["varname"] = "UserInfoPanel"},
    ["GoldBox_Money"] = {["varname"] = "MoneyBg"},
    ["GoldBox_Card"] = {["varname"] = "CardBg"},
    ["HeadBox"] = {["varname"] = "HeadBox"},
    ["ListView_GameList"] = {["varname"] = "GameListView"}
}

-- -----------------------------------------------------
-- MainMsgId            SubMsgId            MsgHandler
-- -----------------------------------------------------
LobbyScene.MainMsgMap = {
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_REQ_GAMEKINDS_SUCC, MsgHandler = "onRequestGameKindsSuccessMsg"},
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_REQ_GAMEKINDS_FAIL, MsgHandler = "onRequestGameKindsFailureMsg"},
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_REQ_GAMEPLACES_SUCC, MsgHandler = "onRequestGamePlacesSuccessMsg"},
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_REQ_GAMEPLACES_FAIL, MsgHandler = "onRequestGamePlacesFailureMsg"}
}


function LobbyScene:onCreate()
    print("-------LobbyScene:onCreate-------")
    local userNameText = self.UserInfoPanel:getChildByName("Text_UserName")
    local userIdText = self.UserInfoPanel:getChildByName("Text_UserId")
    local headImage = self.HeadBox:getChildByName("HeadImg")
    local moneyText = self.MoneyBg:getChildByName("Text_Money")
    local cardText = self.CardBg:getChildByName("Text_Card")

    userNameText:setString(__GData__.GameUser.UserName)
    userIdText:setString(__GData__.GameUser.UserId)
    moneyText:setString(__GData__.GameUser.Money)
    cardText:setString(__GData__.GameUser.RoomCard)

    if __GData__.GameUser.HeadIndex ~= UserHeadIndex.HeadCustom then
        headImage:setTexture(PreDefHeadImg[__GData__.GameUser.HeadIndex])
    end

    self.GameListView:setScrollBarEnabled(false)

    local itemNode = cc.CSLoader:createNode("GameItem.csb")
    self.GameListItem = itemNode:getChildByName("Panel_Item")
    self.GameListItem:retain()
end

function LobbyScene:initSocketMsgMap(sockObj, msgMap)
    for _, entry in ipairs(msgMap) do
        SocketMsgMapper.registerAnMapEntry(sockObj, entry.MainMsgId, entry.SubMsgId, handler(self, self[entry.MsgHandler]))
    end
end

function LobbyScene:unInitSocketMsgMap(sockObj, msgMap)
    for _, entry in ipairs(msgMap) do
        SocketMsgMapper.removeAnMapEntry(sockObj, entry.MainMsgId, entry.SubMsgId)
    end
end

function LobbyScene:onEnter()
    print("-------LobbyScene:onEnter-------")
    self:initSocketMsgMap(__GData__.MainSocket, LobbyScene.MainMsgMap)

    if 0 == #__GData__.GameList then
        __GData__.MainSocket:sendData(packNetMsgHeader(MAINMSGID.MSG_MAINID_GAMELIST, SUBMSGID.MSG_SUBID_REQUEST_GAMEKINDS))
    else
        self:showGameList()
    end
end

function LobbyScene:onExit()
    print("-------LobbyScene:onExit-------")
    self:unInitSocketMsgMap(__GData__.MainSocket, LobbyScene.MainMsgMap)

    self.GameListItem:release()
end

function LobbyScene:showGameList()
    for i = 1, #__GData__.GameList do
        local gameItem = self:createGameItem(__GData__.GameList[i])
        self.GameListView:addChild(gameItem)
    end

    self.GameListView:requestDoLayout()
end

function LobbyScene:onRequestGameKindsSuccessMsg(sockObj, msg, msgLen)
    print("-------onRequestGameKindsSuccessMsg-------")
    __GData__.GameList = parseGameKindListMsg(msg, msgLen)

    for i = 1, #__GData__.GameList do
        __GData__.MainSocket:sendData(packReqGamePlacesMsg(__GData__.GameList[i].KindId))

        local gameItem = self:createGameItem(__GData__.GameList[i])
        self.GameListView:addChild(gameItem)
    end

    self.GameListView:requestDoLayout()
end

function LobbyScene:onRequestGameKindsFailureMsg(sockObj, msg, msgLen)
    print("-------onRequestGameKindsFailureMsg-------")
end

function LobbyScene:onRequestGamePlacesSuccessMsg(sockObj, msg, msgLen)
    print("-------onRequestGamePlacesSuccessMsg-------")
    local placeList = parseGamePlaceListMsg(msg, msgLen)

    for i = 1, #placeList do
        local GameKind = findGameKindById(placeList[i].KindId)
        if GameKind ~= nil then
            GameKind.PlaceList = GameKind.PlaceList or {}

            GameKind.PlaceList[#GameKind.PlaceList + 1] = placeList[i]
        end
    end

    dump(__GData__.GameList, "Game List:", 4)
end

function LobbyScene:onRequestGamePlacesFailureMsg(sockObj, msg, msgLen)
    print("-------onRequestGamePlacesFailureMsg-------")
end

function LobbyScene:createGameItem(gameKind)
    local item = self.GameListItem:clone()
    -- 将GameKind数据记在控件上便于点击时获取相关信息
    item.GameKind_ = gameKind
    item:addClickEventListener(handler(self, self.onGameItemClicked))

    local itemBG = item:getChildByName("ItemBG")
    local itemName = item:getChildByName("Text_Name")
    -- Todo: 设置背景图片
    itemName:setSkewX(10)
    itemName:setString(gameKind.GameName)

    return item
end

function LobbyScene:onGameItemClicked(sender)
    print("------onGameItemClicked------")
    local placeScene = self:getApp():enterScene("PlaceScene")
    placeScene:initWithGameKind(sender.GameKind_)
end

return LobbyScene

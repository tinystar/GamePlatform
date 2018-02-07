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
    ["HeadBox"] = {["varname"] = "HeadBox"}
}

-- -----------------------------------------------------
-- MainMsgId            SubMsgId            MsgHandler
-- -----------------------------------------------------
LobbyScene.MainMsgMap = {
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_REQ_GAMEKINDS_SUCC, MsgHandler = "onRequestGameKindsSuccessMsg"},
    {MainMsgId = MAINMSGID.MSG_MAINID_GAMELIST, SubMsgId = SUBMSGID.MSG_SUBID_REQ_GAMEKINDS_FAIL, MsgHandler = "onRequestGameKindsFailureMsg"}
}


function LobbyScene:onCreate()
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
    self:initSocketMsgMap(__GData__.MainSocket, LobbyScene.MainMsgMap)

    __GData__.MainSocket:sendData(packNetMsgHeader(MAINMSGID.MSG_MAINID_GAMELIST, SUBMSGID.MSG_SUBID_REQUEST_GAMEKINDS))
end

function LobbyScene:onExit()
    self:unInitSocketMsgMap(__GData__.MainSocket, LobbyScene.MainMsgMap)
end

function LobbyScene:onRequestGameKindsSuccessMsg(sockObj, msg, msgLen)
    print("-------onRequestGameKindsSuccessMsg-------")
    __GData__.GameList = parseGameKindListMsg(msg, msgLen)

    for i = 1, #__GData__.GameList do
        print(__GData__.GameList[i].KindId)
        print(__GData__.GameList[i].GameName)
        print(__GData__.GameList[i].ClientMod)
        print(__GData__.GameList[i].GameVer)
    end
end

function LobbyScene:onRequestGameKindsFailureMsg(sockObj, msg, msgLen)
    print("-------onRequestGameKindsFailureMsg-------")
end

return LobbyScene

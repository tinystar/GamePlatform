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

function LobbyScene:onEnter()
end

function LobbyScene:onExit()
end

return LobbyScene

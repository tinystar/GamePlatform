-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/11/4
-- *********************************************************************

local AccountLoginLayer = class("AccountLoginLayer", cc.LayerColor)

-- -----------------------------------------------------
-- MainMsgId            SubMsgId            MsgHandler
-- -----------------------------------------------------
AccountLoginLayer.MainMsgMap = {
    {MainMsgId = MAINMSGID.MSG_MAINID_USER, SubMsgId = SUBMSGID.MSG_SUBID_ACCOUNT_NOT_EXIST, MsgHandler = "onUserAccountNotExistMsg"},
    {MainMsgId = MAINMSGID.MSG_MAINID_USER, SubMsgId = SUBMSGID.MSG_SUBID_WRONG_PASSWORD, MsgHandler = "onUserWrongPasswordMsg"}
}

function AccountLoginLayer:ctor()
    self:onCreate()

    self:registerScriptHandler(function (event)
        if event == "exit" then
            self:onExit()
        end
    end)
end

function AccountLoginLayer:initUI()
    local rootNode = cc.CSLoader:createNodeWithVisibleSize("AccountLoginLayer.csb")
    self:addChild(rootNode)

    local bgNode = rootNode:getChildByName("bg")
    local accountBg = bgNode:getChildByName("account_bg")
    self.accountEdit_ = accountBg:getChildByName("TextField_Account")

    local passwordBg = bgNode:getChildByName("password_bg")
    self.passwordEdit_ = passwordBg:getChildByName("TextField_Password")

    local closeBtn = bgNode:getChildByName("Button_Close")
    closeBtn:addClickEventListener(function(sender)
        self:removeFromParent()
    end)

    local loginBtn = bgNode:getChildByName("Button_Login")
    loginBtn:addClickEventListener(handler(self, self.onLoginBtnClicked))
end

function AccountLoginLayer:initSocketMsgMap(sockObj, msgMap)
    for _, entry in ipairs(msgMap) do
        SocketMsgMapper.registerAnMapEntry(sockObj, entry.MainMsgId, entry.SubMsgId, handler(self, self[entry.MsgHandler]))
    end
end

function AccountLoginLayer:unInitSocketMsgMap(sockObj, msgMap)
    for _, entry in ipairs(msgMap) do
        SocketMsgMapper.removeAnMapEntry(sockObj, entry.MainMsgId, entry.SubMsgId)
    end
end

function AccountLoginLayer:onCreate()
    self:initWithColor(cc.c4b(0,0,0,225))

    self:initUI()
    self:initSocketMsgMap(__GData__.MainSocket, AccountLoginLayer.MainMsgMap)
end

function AccountLoginLayer:onExit()
    self:unInitSocketMsgMap(__GData__.MainSocket, AccountLoginLayer.MainMsgMap)
end

function AccountLoginLayer:onLoginBtnClicked(sender)
    local account = self.accountEdit_:getString()
    local password = self.passwordEdit_:getString()
    __GData__.MainSocket:sendData(packAccountLoginMsg(account, password))
end

-- -----------------------------------------------------
-- MainServer msg callback functions
-- -----------------------------------------------------
function AccountLoginLayer:onUserAccountNotExistMsg(sockObj, msg, msgLen)
    print("-------onUserAccountNotExistMsg-------")
end

function AccountLoginLayer:onUserWrongPasswordMsg(sockObj, msg, msgLen)
    print("-------onUserWrongPasswordMsg-------")
end

return AccountLoginLayer

-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/10/20
-- *********************************************************************

local PromptLayer = import(".PromptLayer")

local LoginScene = class("LoginScene", cc.load("mvc").ViewBase)

LoginScene.RESOURCE_FILENAME = "LoadScene.csb"
LoginScene.RESOURCE_BINDING = {
    ["Panel_Login"] = {["varname"] = "LoginPanel"},
    ["Loading_Node"] = {["varname"] = "LoadingNode"}
}


function LoginScene:onCreate()
    self.LoadingAct = cc.CSLoader:createTimeline("LoadingAnim.csb")
    self.resourceNode_:runAction(self.LoadingAct)
end

function LoginScene:onEnter()
    local connSock = ClientSocketMgr.createSocket("connectSock")
    connSock:addConnectEventListener(handler(self, LoginScene.onSocketGateConnect))
    connSock:addReceiveEventListener(handler(self, LoginScene.onSocketGateMsgReceived))
    connSock:connect(GameConfig.GateSvrAddr, GameConfig.GateSvrPort)

    self.connectingGate_ = true
    self.playLoadingAnim_ = false
    local callbackEntry = nil
    local function startLoadingAnimation(dt)
        if self.connectingGate_ then
            -- play loading animation
            self.LoadingNode:setVisible(true)
            self.LoadingAct:gotoFrameAndPlay(0, 50, true)
            self.playLoadingAnim_ = true
        end

        -- called only once
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(callbackEntry)
    end

    callbackEntry = cc.Director:getInstance():getScheduler():scheduleScriptFunc(startLoadingAnimation, 1, false)
end

-- socket event handlers
function LoginScene:onSocketGateConnect(sockObj, success, errMsg)
    print("--------onSocketConnected-----------", success, errMsg)
    self.connectingGate_ = false
    if self.playLoadingAnim_ then
        --self.resourceNode_:stopAction(self.LoadingAct)
        self.LoadingAct:stop()
        self.LoadingNode:setVisible(false)
    end

    if success then
        sockObj:sendData(packNetMsgHeader(MAINMSGID.MSG_MAINID_GATE, SUBMSGID.MSG_SUBID_REQUEST_CONFIG))
    else
        local promptLayer = PromptLayer:create(gres.str.CONNECT_GATE_FAILED .. errMsg)
        self:addChild(promptLayer)
    end
end

function LoginScene:onSocketGateMsgReceived(sockObj, data, len)
    local mainId, subId, msg, msgLen = parseNetMsgHeader(data, len)
    print(string.format("recv gate server msg: %d %d  %d", mainId, subId,  msgLen))
    assert(MAINMSGID.MSG_MAINID_GATE == mainId, "Invalid msg header, wrong main msg id!")

    if SUBMSGID.MSG_SUBID_CONFIG == subId then
        parseGateConfigMsg(msg, msgLen)
    elseif SUBMSGID.MSG_SUBID_MAINSVR_ADDR == subId then

    elseif SUBMSGID.MSG_SUBID_NO_MAINSVR == subId then

    end
end

return LoginScene

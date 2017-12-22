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
local AccountLoginLayer = import(".AccountLoginLayer")
local ViewMainMsgMapper = import(".ViewMainMsgMapper")

local LoginScene = class("LoginScene", cc.load("mvc").ViewBase, ViewMainMsgMapper)

LoginScene.RESOURCE_FILENAME = "LoadScene.csb"
LoginScene.RESOURCE_BINDING = {
    ["Panel_Login"] = {["varname"] = "LoginPanel"},
    ["Loading_Node"] = {["varname"] = "LoadingNode"}
}

-- -----------------------------------------------------
-- MainMsgId            SubMsgId            MsgHandler
-- -----------------------------------------------------
LoginScene.MainMsgMap = {
    {MainMsgId = MAINMSGID.MSG_MAINID_USER, SubMsgId = SUBMSGID.MSG_SUBID_LOGIN_SUCCESS, MsgHandler = "onUserLoginSuccessMsg"}
}


function LoginScene:onCreate()
    self.LoadingAct = cc.CSLoader:createTimeline("LoadingAnim.csb")
    self.resourceNode_:runAction(self.LoadingAct)

    local weChatLoginBtn = self.LoginPanel:getChildByName("Button_WeChat")
    weChatLoginBtn:addClickEventListener(handler(self, self.onWeChatLoginBtnClicked))
    local accountLoginBtn = self.LoginPanel:getChildByName("Button_Account")
    accountLoginBtn:addClickEventListener(handler(self, self.onAccountLoginBtnClicked))
end

function LoginScene:initEventListeners()
    self.eventListeners_ = {}

    local eventDispatcher = self:getEventDispatcher()
    local listener = cc.EventListenerCustom:create(CustEvents.PromptRemove, handler(self, self.onPromptLayerRemoved))
    eventDispatcher:addEventListenerWithFixedPriority(listener, 1)
    table.insert(self.eventListeners_, listener)

    listener = cc.EventListenerCustom:create(CustEvents.ConnMainFail, handler(self, self.onEventMainServerConnectFailed))
    eventDispatcher:addEventListenerWithFixedPriority(listener, 1)
    table.insert(self.eventListeners_, listener)
end

function LoginScene:cleanupEventListeners()
    for i, listener in ipairs(self.eventListeners_) do
        self:getEventDispatcher():removeEventListener(listener)
    end

    self.eventListeners_ = nil
end

function LoginScene:connectGateServer()
    local gateSock = ClientSocketMgr.createSocket("gateSocket")
    gateSock:addConnectEventListener(handler(self, LoginScene.onSocketGateConnect))
    gateSock:addReceiveEventListener(handler(self, LoginScene.onSocketGateMsgReceived))
    gateSock:connect(GameConfig.GateSvrAddr, GameConfig.GateSvrPort)
end

function LoginScene:startConnectingAnimation()
    self.connecting_ = true
    self.playLoadingAnim_ = false
    local callbackEntry = nil
    local function startLoadingAnimation(dt)
        if self.connecting_ then
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

function LoginScene:stopConnectingAnimation()
    self.connecting_ = false
    if self.playLoadingAnim_ then
        --self.resourceNode_:stopAction(self.LoadingAct)
        self.LoadingAct:stop()
        self.LoadingNode:setVisible(false)
    end
end

function LoginScene:onEnter()
    self:initEventListeners()
    self:initMainMsgMap(LoginScene.MainMsgMap)

    -- connect to gate server
    self:connectGateServer()
    self:startConnectingAnimation()
end

function LoginScene:onExit()
    self:unInitMainMsgMap()
    self:cleanupEventListeners()
end

function LoginScene:onWeChatLoginBtnClicked(sender)
    print("-----LoginScene:onWeChatLoginBtnClicked-----")
end

function LoginScene:onAccountLoginBtnClicked(sender)    
    local loginLayer = AccountLoginLayer:create()
    self:addChild(loginLayer)
end

function LoginScene:onPromptLayerRemoved(event)
    print("-----LoginScene:onPromptLayerRemoved-----")
end

-- override ViewMainMsgMapper's method
function LoginScene:onEventMainServerConnectSuccessful(event)
    -- call super method
    ViewMainMsgMapper.onEventMainServerConnectSuccessful(self, event)

    self:stopConnectingAnimation()

    self.LoginPanel:setVisible(true)
end

function LoginScene:onEventMainServerConnectFailed(event)
    self:stopConnectingAnimation()

    local promptLayer = PromptLayer:create(gres.str.CONNECT_MAIN_FAILED .. event._usedata)
    self:addChild(promptLayer)
end

-- -----------------------------------------------------
-- MainServer msg callback functions
-- -----------------------------------------------------
function LoginScene:onUserLoginSuccessMsg(sockObj, msg, msgLen)
    print("-------onUserLoginSuccessMsg-------")
end

-- gate socket event handlers
function LoginScene:onSocketGateConnect(sockObj, success, errMsg)
    self:stopConnectingAnimation()

    if success then
        sockObj:sendData(packNetMsgHeader(MAINMSGID.MSG_MAINID_GATE, SUBMSGID.MSG_SUBID_REQUEST_CONFIG))
    else
        local promptLayer = PromptLayer:create(gres.str.CONNECT_GATE_FAILED .. errMsg)
        self:addChild(promptLayer)
    end
end

function LoginScene:onGateConfigMsg(sockObj, msg, msgLen)
    local gateConfig = parseGateConfigMsg(msg, msgLen)
    print("Gate Config Message:")
    printf("version = %s", gateConfig.version)
    printf("update url = %s", gateConfig.updateUrl)

    -- need update
    if gateConfig.version > GameConfig.Version then
        -- Todo: update to version
    end

    -- request main server address
    sockObj:sendData(packNetMsgHeader(MAINMSGID.MSG_MAINID_GATE, SUBMSGID.MSG_SUBID_REQUEST_MAINSVR_ADDR))
end

function LoginScene:onMainAddressMsg(sockObj, msg, msgLen)
    local mainAddr = parseMainAddressMsg(msg, msgLen)
    print("Main Address Message:")
    printf("ip = %s", mainAddr.mainAddr)
    printf("port = %d", mainAddr.mainPort)

    -- close and remove gate socket
    sockObj:close()
    ClientSocketMgr.removeSocket("gateSocket")

    __GData__.MainSvrAddress = mainAddr.mainAddr
    __GData__.MainSvrPort = mainAddr.mainPort

    -- connect to main server
    connectMainServer()
    self:startConnectingAnimation()
end

function LoginScene:onNoMainServerMsg(sockObj, msg, msgLen)
    local promptLayer = PromptLayer:create(gres.str.NO_MAIN_SERVER)
    self:addChild(promptLayer)
end

function LoginScene:onSocketGateMsgReceived(sockObj, data, len)
    local mainId, subId, msg, msgLen = parseNetMsgHeader(data, len)
    print(string.format("recv gate server msg: %d %d  %d", mainId, subId,  msgLen))
    assert(MAINMSGID.MSG_MAINID_GATE == mainId, "Invalid msg header, wrong main msg id!")

    if SUBMSGID.MSG_SUBID_CONFIG == subId then
        self:onGateConfigMsg(sockObj, msg, msgLen)
    elseif SUBMSGID.MSG_SUBID_MAINSVR_ADDR == subId then
        self:onMainAddressMsg(sockObj, msg, msgLen)
    elseif SUBMSGID.MSG_SUBID_NO_MAINSVR == subId then
        self:onNoMainServerMsg(sockObj, msg, msgLen)
    end
end

return LoginScene

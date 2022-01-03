-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/11/1
-- *********************************************************************

require "app.model.GData"
require "app.model.GameConstDefs"
require "app.network.SocketMsgMapper"
require "app.network.SocketMsgHandler"

-- -----------------------------------------------------
-- Message Notifications
-- -----------------------------------------------------
local function onMainServerConnect(sockObj, success, errMsg)
    stopConnectingAnimation()

    local eventDispatcher = cc.Director:getInstance():getEventDispatcher()
    if success then
        local event = cc.EventCustom:new(CustEvents.ConnMainSucc)
        eventDispatcher:dispatchEvent(event)
    else
        local event = cc.EventCustom:new(CustEvents.ConnMainFail)
        event._usedata = errMsg
        eventDispatcher:dispatchEvent(event)
    end
end

local function onMainServerReceived(sockObj, data, len)
    SocketMsgMapper.onSocketMsgRecv(sockObj, data, len)
end

local function onMainServerClose(sockObj, reason)
    local eventDispatcher = cc.Director:getInstance():getEventDispatcher()
    local event = cc.EventCustom:new(CustEvents.MainSockClose)
    event._usedata = reason
    eventDispatcher:dispatchEvent(event)

    if reason ~= nil and reason ~= "connect failed" then   -- 不是主动关闭连接或不是连接过程中失败
        connectMainServer() -- 断线重连
    end
end

local function onRoomServerConnect(sockObj, success, errMsg)
    stopConnectingAnimation()

    local eventDispatcher = cc.Director:getInstance():getEventDispatcher()
    if success then
        local event = cc.EventCustom:new(CustEvents.ConnRoomSucc)
        eventDispatcher:dispatchEvent(event)
    else
        local event = cc.EventCustom:new(CustEvents.ConnRoomFail)
        event._usedata = errMsg
        eventDispatcher:dispatchEvent(event)
    end
end

local function onRoomServerReceived(sockObj, data, len)
    SocketMsgMapper.onSocketMsgRecv(sockObj, data, len)
end

local function onRoomServerClose(sockObj, reason)
    local eventDispatcher = cc.Director:getInstance():getEventDispatcher()
    local event = cc.EventCustom:new(CustEvents.RoomSockClose)
    event._usedata = reason
    eventDispatcher:dispatchEvent(event)

    if reason ~= nil and reason ~= "connect failed" then   -- 不是主动关闭连接或不是连接过程中失败
        connectRoomServer() -- 断线重连
    end
end

-- -----------------------------------------------------
-- Connecting Animation
-- -----------------------------------------------------
function startConnectingAnimation()
    local runningScene = cc.Director:getInstance():getRunningScene()

    -- 触摸屏蔽层
    local touchLayer = cc.Layer:create()
    touchLayer:onTouch(function(event)
        if event.name == "began" then
            return true
        end
    end, false, true)

    runningScene.touchLayer_ = touchLayer
    runningScene:addChild(touchLayer)

    local animNode = cc.CSLoader:createNode("LoadingAnim.csb")
    local actNode = cc.CSLoader:createTimeline("LoadingAnim.csb")
    touchLayer:addChild(animNode)
    animNode:setVisible(false)
    animNode:setPosition(display.center)
    touchLayer:runAction(actNode)

    runningScene.connecting_ = true
    local callbackEntry = nil
    local function startLoadingAnimation(dt)
        if runningScene.connecting_ then
            -- play loading animation
            animNode:setVisible(true)
            actNode:gotoFrameAndPlay(0, 50, true)
        end

        -- called only once
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(callbackEntry)
    end

    callbackEntry = cc.Director:getInstance():getScheduler():scheduleScriptFunc(startLoadingAnimation, 1, false)
end

function stopConnectingAnimation()
    local runningScene = cc.Director:getInstance():getRunningScene()

    runningScene.connecting_ = false
    runningScene.touchLayer_:removeFromParent()
end

-- -----------------------------------------------------
-- Main socket
-- -----------------------------------------------------
function createMainSocket()
    if nil == __GData__.MainSocket then
        local mainSock = ClientSocketMgr.createSocket("mainSocket")
        mainSock:addConnectEventListener(onMainServerConnect)
        mainSock:addReceiveEventListener(onMainServerReceived)
        mainSock:addCloseEventListener(onMainServerClose)

        -- map main server net message when main socket object created. 
        mapMainServerNetMsg(mainSock)
        __GData__.MainSocket = mainSock
    end

    return __GData__.MainSocket
end

function connectMainServer()
    if nil == __GData__.MainSocket then
        createMainSocket()
    end

    if __GData__.MainSocket:isConnected() then
        return
    end

    if __GData__.MainSvrAddress == nil or
       __GData__.MainSvrPort == nil then
       return
    end

    __GData__.MainSocket:connect(__GData__.MainSvrAddress, __GData__.MainSvrPort)
    startConnectingAnimation()
end

function cleanMainSocket()
    if nil == __GData__.MainSocket then
        return
    end

    __GData__.MainSocket.removeConnectEventListener(onMainServerConnect)
    __GData__.MainSocket.removeReceiveEventListener(onMainServerReceived)
    __GData__.MainSocket.removeCloseEventListener(onMainServerClose)

    unmapMainServerNetMsg(__GData__.MainSocket)

    __GData__.MainSocket:close()
    ClientSocketMgr.removeSocket("mainSocket")
    __GData__.MainSocket = nil
end

-- -----------------------------------------------------
-- Room socket
-- -----------------------------------------------------
function createRoomSocket()
    if nil == __GData__.RoomSocket then
        local roomSock = ClientSocketMgr.createSocket("roomSocket")
        roomSock:addConnectEventListener(onRoomServerConnect)
        roomSock:addReceiveEventListener(onRoomServerReceived)
        roomSock:addCloseEventListener(onRoomServerClose)

        -- map room server net message when room socket object created. 
        mapRoomServerNetMsg(roomSock)
        __GData__.RoomSocket = roomSock
    end

    return __GData__.RoomSocket
end

function connectRoomServer()
    if nil == __GData__.RoomSocket then
        createRoomSocket()
    end

    if __GData__.RoomSocket:isConnected() then
        return
    end

    if __GData__.RoomSvrAddress == nil or
       __GData__.RoomSvrPort == nil then
       return
    end

    __GData__.RoomSocket:connect(__GData__.RoomSvrAddress, __GData__.RoomSvrPort)
    startConnectingAnimation()
end

function cleanRoomSocket()
    if nil == __GData__.RoomSocket then
        return
    end

    __GData__.RoomSocket.removeConnectEventListener(onRoomServerConnect)
    __GData__.RoomSocket.removeReceiveEventListener(onRoomServerReceived)
    __GData__.RoomSocket.removeCloseEventListener(onRoomServerClose)

    unmapRoomServerNetMsg(__GData__.RoomSocket)

    __GData__.RoomSocket:close()
    ClientSocketMgr.removeSocket("roomSocket")
    __GData__.RoomSocket = nil
end

-- -----------------------------------------------------
-- Misc
-- -----------------------------------------------------
function findGameKindById(kindId)
    for i = 1, #__GData__.GameList do
        if __GData__.GameList[i].KindId == kindId then
            return __GData__.GameList[i]
        end
    end

    return nil
end
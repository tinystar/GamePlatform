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

local function onMainServerConnect(sockObj, success, errMsg)
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

    if reason ~= nil then   -- 不是主动关闭连接
        connectMainServer() -- 断线重连
    end
end

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

function findGameKindById(kindId)
    for i = 1, #__GData__.GameList do
        if __GData__.GameList[i].KindId == kindId then
            return __GData__.GameList[i]
        end
    end

    return nil
end
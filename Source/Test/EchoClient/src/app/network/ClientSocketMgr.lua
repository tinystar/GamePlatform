-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/7/8
-- *********************************************************************

require "socket"


local M = {}
ClientSocketMgr = M

local function notifyClientSockRead(readsocks)
    for _, s in ipairs(readsocks) do
        for k, clientsock in pairs(M.clientSockets_) do
            if s == clientsock.s_ then
                clientsock:onSocketRead()
            end
        end
    end
end

local function notifyClientSockWrite(writesocks)
    for _, s in ipairs(writesocks) do
        for k, clientsock in pairs(M.clientSockets_) do
            if s == clientsock.s_ then
                clientsock:onSocketWrite()
            end
        end
    end
end

local function socketEventUpdater(dt)
    local socktbl = {}
    for _, clientsock in pairs(M.clientSockets_) do
        if clientsock.s_ ~= nil then
            socktbl[#socktbl + 1] = clientsock.s_
        end
    end

    if #socktbl <= 0 then
        return
    end

    local readsocks, writesocks, msg = socket.select(socktbl, socktbl, 0)

    if writesocks ~= nil and #writesocks > 0 then
        notifyClientSockWrite(writesocks)
    end
    if readsocks ~= nil and #readsocks > 0 then
        notifyClientSockRead(readsocks)
    end
end

function M.init()
    M.clientSockets_ = M.clientSockets_ or {}

    local scheduler = cc.Director:getInstance():getScheduler()
    M.timerId_ = scheduler:scheduleScriptFunc(socketEventUpdater, 0, false)
end

function M.createSocket(name)
    assert(M.clientSockets_, "please call init function first!")

    local clientSockObj = require("app.network.ClientSocket"):create()
    if clientSockObj then 
        M.clientSockets_[name] = clientSockObj
    end

    return clientSockObj
end

function M.removeSocket(name)
    M.clientSockets_[name] = nil
end

function M.getSocket(name)
    return M.clientSockets_[name]
end

function M.closeAllSockets()
    for k, clientsock in pairs(M.clientSockets_) do
        if clientsock.s_ ~= nil then
            clientsock:close()
        end
    end
end

return M

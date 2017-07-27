-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/7/8
-- *********************************************************************

local socket = require "socket"


local ClientSocket = class("ClientSocket")

function ClientSocket:ctor()
    -- create tcp socket
    local s, msg = socket.tcp()
    if not s then
        error(string.format("create tcp socket fails, error msg: (%s)", msg))
    end
    s:settimeout(0)
    self.s_ = s

    self.isConnected_ = false

    self.connectListeners_ = {}
    self.receiveListeners_ = {}

    self.recvBuffer_ = ""
    self.sendBuffer_ = ""
end

function ClientSocket:socket()
    return self.s_
end

function ClientSocket:addConnectEventListener(func)
    if func ~= nil then
        table.insert(self.connectListeners_, func)
    end
end

function ClientSocket:removeConnectEventListener(func)
    for i, v in ipairs(self.connectListeners_) do
        if func == v then
            table.remove(self.connectListeners_, i)
        end
    end
end

function ClientSocket:addReceiveEventListener(func)
    if func ~= nil then
        table.insert(self.receiveListeners_, func)
    end
end

function ClientSocket:removeReceiveEventListener(func)
    for i, v in ipairs(self.receiveListeners_) do
        if func == v then
            table.remove(self.receiveListeners_, i)
        end
    end
end

function ClientSocket:connect(host, port)
    local status, msg = self.s_:connect(host, port)
    if status == 1 then
        self.isConnected_ = true
    else
        print(string.format("connect msg: %s", msg))
    end
end

function ClientSocket:sendData(data, len)
    if not self.isConnected_ then
        return
    end

    if nil == data or 0 == #data then
        return
    end

    local sendPkg
    if nil == len then
        sendPkg = string.pack("IA", #data, data)
    else
        if len <= 0 then return end

        len = math.min(len, #data)
        sendPkg = string.pack("IA", len, string.sub(data, 1, len))
    end

    self.sendBuffer_ = self.sendBuffer_ .. sendPkg
end

function ClientSocket:close()
    self.s_:close()
    self.s_ = nil

    self.isConnected_ = false
    self.connectListeners_ = nil
    self.receiveListeners_ = nil
    self.recvBuffer_ = nil
    self.sendBuffer_ = nil
end

function ClientSocket:onSocketRead()
    local s, msg, partial = self.s_:receive('*a')

    local recvData = s or partial
    -- 服务端关闭socket时，windows平台下msg为"timeout"，此时收到的数据为0长度
    -- 将接收到0长度数据也做为socket关闭的条件
    if "closed" == msg or nil == recvData or 0 == #recvData then
        self:close()
        return
    end

    self.recvBuffer_ = self.recvBuffer_ .. recvData
    if #self.recvBuffer_ <= 4 then
        return
    end

    while true do
        local nextPos, pkgSize = string.unpack(self.recvBuffer_, "I")
        if nil == pkgSize then break end

        if pkgSize + 4 > #self.recvBuffer_ then
            break
        end

        local pkgRecv = string.sub(self.recvBuffer_, nextPos, nextPos + pkgSize - 1)
        for i, listener in ipairs(self.receiveListeners_) do
            listener(self, pkgRecv, pkgSize)
        end

        self.recvBuffer_ = string.sub(self.recvBuffer_, nextPos + pkgSize)
    end
end

function ClientSocket:onSocketWrite()
    -- connected
    if not self.isConnected_ then
        self.isConnected_ = true

        -- notify all connect event listeners
        for _, func in ipairs(self.connectListeners_) do
            func(self)
        end

        return
    end

    -- send data
    if 0 == #self.sendBuffer_ then
        return
    end

    local sIdx, msg, aIdx = self.s_:send(self.sendBuffer_)
    if "closed" == msg then
        self:close()
        return
    end

    local sendSize = sIdx or aIdx
    if 0 == sendSize then return end

    self.sendBuffer_ = string.sub(self.sendBuffer_, sendSize + 1, -1)
end

return ClientSocket

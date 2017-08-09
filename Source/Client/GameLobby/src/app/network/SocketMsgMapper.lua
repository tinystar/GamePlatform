-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/7/19
-- *********************************************************************

local M = {}
SocketMsgMapper = M

function M.onSocketMsgRecv(sockObj, data, len)
    local nextPos, mainId, subId, _ = string.unpack(data, "HHI")

    if nil == M.SockMsgMap_ or nil == M.SockMsgMap_[sockObj] then
        return
    end

    local mainMsgMap = M.SockMsgMap_[sockObj][mainId]
    if nil == mainMsgMap then
        return
    end

    local msghandler = mainMsgMap[subId]
    if msghandler ~= nil then
        local msg = nil
        local msgLen = 0
        if nextPos < len then
            msg = string.sub(data, nextPos, len)
            msgLen = len - nextPos
        end
        msghandler(sockObj, msg, msgLen)
    end
end

function M.registerMsgHandler(sockObj, msgMapArray)
    if nil == sockObj or nil == msgMapArray then
        return
    end

    M.SockMsgMap_ = M.SockMsgMap_ or {}
    M.SockMsgMap_[sockObj] = M.SockMsgMap_[sockObj] or {}

    for i = 1, #msgMapArray do
        local msgItem = msgMapArray[i]
        local mainId = msgItem.MainMsgId
        local subId = msgItem.SubMsgId
        local msghandler = msgItem.MsgHandler
        assert(msghandler, "message handler function must not be nil!")

        M.SockMsgMap_[sockObj][mainId] = M.SockMsgMap_[sockObj][mainId] or {}
        M.SockMsgMap_[sockObj][mainId][subId] = msghandler
    end
end

function M.removeMsgHandler(sockObj, msgMapArray)
    if nil == sockObj then
        return
    end

    if nil == M.SockMsgMap_[sockObj] then
        return
    end

    for i = 1, #msgMapArray do
        local msgItem = msgMapArray[i]
        local mainId = msgItem.MainMsgId
        local subId = msgItem.SubMsgId

        if M.SockMsgMap_[sockObj][mainId] ~= nil then
            M.SockMsgMap_[sockObj][mainId][subId] = nil
        end
    end
end

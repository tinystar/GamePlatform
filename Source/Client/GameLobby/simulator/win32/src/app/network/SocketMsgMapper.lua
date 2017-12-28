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
    local mainId, subId, msg, msgLen = parseNetMsgHeader(data, len)

    if nil == M.SockMsgMap_ or nil == M.SockMsgMap_[sockObj] then
        return
    end

    local mainMsgMap = M.SockMsgMap_[sockObj][mainId]
    if nil == mainMsgMap then
        return
    end

    local msghandler = mainMsgMap[subId]
    if msghandler ~= nil then
        msghandler(sockObj, msg, msgLen)
    end
end

function M.registerMsgHandler(sockObj, msgMapArray)
    if nil == sockObj or nil == msgMapArray then
        return
    end

    for i = 1, #msgMapArray do
        local msgItem = msgMapArray[i]
        local mainId = msgItem.MainMsgId
        local subId = msgItem.SubMsgId
        local msghandler = msgItem.MsgHandler
        assert(msghandler, "message handler function must not be nil!")

        M.registerAnMapEntry(sockObj, mainId, subId, msghandler)
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

        M.removeAnMapEntry(sockObj, mainId, subId)
    end
end

function M.registerAnMapEntry(sockObj, mainId, subId, msgHandler)
    if nil == sockObj then
        return
    end

    if nil == mainId or nil == subId or nil == msgHandler then
        return
    end

    M.SockMsgMap_ = M.SockMsgMap_ or {}
    M.SockMsgMap_[sockObj] = M.SockMsgMap_[sockObj] or {}
    M.SockMsgMap_[sockObj][mainId] = M.SockMsgMap_[sockObj][mainId] or {}
    if M.SockMsgMap_[sockObj][mainId][subId] ~= nil then
        print(string.format("message mapping already exists with main id = %d and sub id = %d", mainId, subId))
        return
    end

    M.SockMsgMap_[sockObj][mainId][subId] = msgHandler
end

function M.removeAnMapEntry(sockObj, mainId, subId)
    if nil == sockObj then
        return
    end

    if nil == M.SockMsgMap_[sockObj] then
        return
    end

    if M.SockMsgMap_[sockObj][mainId] ~= nil then
        M.SockMsgMap_[sockObj][mainId][subId] = nil
    end
end

return M

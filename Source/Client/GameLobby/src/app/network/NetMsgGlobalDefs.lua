-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/10/20
-- *********************************************************************

MAINMSGID = {
    MSG_MAINID_GATE                 = 1
}

SUBMSGID = {
    -- GateServer Msg
    MSG_SUBID_REQUEST_CONFIG        = 1,
    MSG_SUBID_REQUEST_MAINSVR_ADDR  = 2,
    MSG_SUBID_CONFIG                = 1,
    MSG_SUBID_MAINSVR_ADDR          = 2,
    MSG_SUBID_NO_MAINSVR            = 3
}

function parseNetMsgHeader(data, len)
    -- the msg header size is 8.
    if len < 8 then
        return nil
    end

    local nextPos, mainId, subId, _ = string.unpack(data, "HHI")

    local msg = nil
    local msgLen = 0
    if len > 8 then
        msg = string.sub(data, nextPos, len)
        msgLen = len - 8
    end

    return mainId, subId, msg, msgLen
end

function packNetMsgHeader(mainId, subId, reserved)
    reserved = reserved or 0

    local header = string.pack("HHI", mainId, subId, reserved)
    return header, #header
end

function parseGateConfigMsg(data, len)
    assert(144 == len, "Invalid Gate Config Msg!")
    local nextPos, ver, updUrl = string.unpack(data, "A16A128")

    print(ver)
    print(updUrl)
end

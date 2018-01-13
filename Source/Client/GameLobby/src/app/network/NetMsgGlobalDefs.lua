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
    MSG_MAINID_GATE                 = 1,
    MSG_MAINID_USER                 = 10
}

SUBMSGID = {
    -- **********GateServer Msg***********
    -- Client -> Server
    MSG_SUBID_REQUEST_CONFIG        = 1,
    MSG_SUBID_REQUEST_MAINSVR_ADDR  = 2,
    -- Server -> Client
    MSG_SUBID_CONFIG                = 1,
    MSG_SUBID_MAINSVR_ADDR          = 2,
    MSG_SUBID_NO_MAINSVR            = 3,

    -- **********MainServer Msg***********
    -- Client -> Server
    MSG_SUBID_ACCOUNT_LOGIN         = 1,
    MSG_SUBID_WECHAT_LOGIN          = 2,
    MSG_SUBID_QUICK_LOGIN           = 3,
    -- Server -> Client
    MSG_SUBID_LOGIN_SUCCESS         = 1,
    MSG_SUBID_LOGIN_FAILURE         = 2
}

-- -----------------------------------------------------
-- GameMsgHeader
--     uMainId      -   UINT16
--     uSubId       -   UINT16
--     uReserved    -   UINT32
-- -----------------------------------------------------
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

-- -----------------------------------------------------
-- GateConfigMsg
--     szVersion    -   char[16]
--     szUpdUrl     -   char[128]
-- -----------------------------------------------------
function parseGateConfigMsg(data, len)
    assert(144 == len, "Invalid Gate Config Msg!")
    local nextPos, ver, updUrl = string.unpack(data, "A16A128")

    return {version = ver, updateUrl = updUrl}
end

-- -----------------------------------------------------
-- MainAddressMsg
--     szIP         -   char[20]
--     sPort        -   UINT16
-- -----------------------------------------------------
function parseMainAddressMsg(data, len)
    assert(22 == len, "Invalid Main Address Msg!")
    local nextPos, addr, port = string.unpack(data, "A20H")

    return {mainAddr = addr, mainPort = port}
end

-- -----------------------------------------------------
-- AccountLoginMsg
--     szAccount    -   char[64]
--     szPassword   -   char[16]
-- -----------------------------------------------------
function packAccountLoginMsg(account, password)
    local header, headerLen = packNetMsgHeader(MAINMSGID.MSG_MAINID_USER, SUBMSGID.MSG_SUBID_ACCOUNT_LOGIN)

    local msg = string.pack("AAA", header, paddingWith(account, 64), paddingWith(password, 16))
    return msg, #msg
end

-- -----------------------------------------------------
-- UserInfoMsg
--     userId       -   UINT32
--     szAccount    -   char[17]
--     szUserName   -   char[65]
--     headIdx      -   INT32
--     genderType   -   INT32
--     uMoney       -   UINT32
--     uRoomCard    -   UINT32
--     szPhoneNum   -   char[16]
--     uTypeFlag    -   UINT32
-- -----------------------------------------------------
function parseUserInfoMsg(data, len)
    assert(122 == len, "Invalid User Information Msg!")
    local nextPos, userId, account, userName, headIdx, gender, money, roomCard, phoneNumber, flag = string.unpack(data, "IA17A65iiIIA16I")
    money = money / 100

    return {UserId = userId,
            Account = account,
            UserName = userName,
            HeadIndex = headIdx,
            Gender = gender,
            Money = money,
            RoomCard = roomCard,
            PhoneNumber = phoneNumber,
            TypeFlag = flag
            }
end

-- -----------------------------------------------------
-- LoginFailMsg
--     nFailReason  -   INT32
-- -----------------------------------------------------
function parseLoginFailMsg(data, len)
    assert(4 == len, "Invalid Login Fail Msg!")
    local nextPos, reason = string.unpack(data, "iA")

    return {FailReason = reason}
end

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
    MSG_MAINID_USER                 = 10,
    MSG_MAINID_GAMELIST             = 11
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
    MSG_SUBID_REQUEST_GAMEKINDS     = 1,
    MSG_SUBID_REQUEST_GAMEPLACES    = 2,
    MSG_SUBID_ENTER_GAMEPLACE       = 3,
    -- Server -> Client
    MSG_SUBID_LOGIN_SUCCESS         = 1,
    MSG_SUBID_LOGIN_FAILURE         = 2,
    MSG_SUBID_REQ_GAMEKINDS_SUCC    = 1,
    MSG_SUBID_REQ_GAMEKINDS_FAIL    = 2,
    MSG_SUBID_REQ_GAMEPLACES_SUCC   = 3,
    MSG_SUBID_REQ_GAMEPLACES_FAIL   = 4,
    MSG_SUBID_ROOM_ADDRESS          = 5,
    MSG_SUBID_ENTER_PLACE_FAIL      = 6
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
-- QuickLoginMsg
--     deviceInfo   -   UserDeviceInfo
--                          szOS         -   char[24]
--                          szDevice   -     char[64]
-- -----------------------------------------------------
function packQuickLoginMsg(os, device)
    local header, headerLen = packNetMsgHeader(MAINMSGID.MSG_MAINID_USER, SUBMSGID.MSG_SUBID_QUICK_LOGIN)

    local msg = string.pack("AAA", header, paddingWith(os, 24), paddingWith(device, 64))
    return msg, #msg
end

-- -----------------------------------------------------
-- AccountLoginMsg
--     szAccount    -   char[64]
--     szPassword   -   char[16]
--     deviceInfo   -   UserDeviceInfo
--                          szOS         -   char[24]
--                          szDevice   -     char[64]
-- -----------------------------------------------------
function packAccountLoginMsg(account, password, os, device)
    local header, headerLen = packNetMsgHeader(MAINMSGID.MSG_MAINID_USER, SUBMSGID.MSG_SUBID_ACCOUNT_LOGIN)

    local msg = string.pack("AAAAA", header, paddingWith(account, 64), paddingWith(password, 16),
                                             paddingWith(os, 24), paddingWith(device, 64))
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
    local nextPos, reason = string.unpack(data, "i")

    return {FailReason = reason}
end

-- -----------------------------------------------------
-- GameKindListMsg
--     uCount       -   INT32
--     kinds        -   GameKindMsgInfo
--                          nKindId         -   INT32
--                          szGameName      -   char[24]
--                          szCliModInfo    -   char[24]
--                          szSvrModInfo    -   char[24]
--                          szVersion       -   char[16]
-- -----------------------------------------------------
function parseGameKindListMsg(data, len)
    local nextPos, count = string.unpack(data, "i")

    local gameList = {}
    for i = 1, count do
        local _next, kindId, gameName, cliModeInfo, svrModInfo, version = string.unpack(data, "iA24A24A24A16", nextPos)
        nextPos = _next

        gameList[i] = {KindId = kindId,
                       GameName = gameName,
                       ClientMod = cliModeInfo,
                       GameVer = version
                      }
    end

    return gameList
end

-- -----------------------------------------------------
-- ReqGamePlacesMsg
--     nKindId      -   INT32
-- -----------------------------------------------------
function packReqGamePlacesMsg(kindId)
    local header, headerLen = packNetMsgHeader(MAINMSGID.MSG_MAINID_GAMELIST, SUBMSGID.MSG_SUBID_REQUEST_GAMEPLACES)

    local msg = string.pack("Ai", header, kindId)
    return msg, #msg
end

-- -----------------------------------------------------
-- GamePlaceListMsg
--     uCount       -   INT32
--     places       -   GamePlaceMsgInfo
--                          nKindId         -   INT32
--                          nPlaceId        -   INT32
--                          szPlaceName     -   char[24]
--                          nPlaceType      -   INT32
--                          uEnterLimit     -   UINT32
--                          uBasePoint      -   UINT32
-- -----------------------------------------------------
function parseGamePlaceListMsg(data, len)
    local nextPos, count = string.unpack(data, "i")

    local placeList = {}
    for i = 1, count do
        local _next, kindId, placeId, placeName, placeType, enterLimit, basePoint = string.unpack(data, "iiA24iII", nextPos)
        nextPos = _next

        enterLimit = enterLimit / 100
        basePoint = basePoint / 100
        placeList[i] = {KindId = kindId,
                        PlaceId = placeId,
                        PlaceName = placeName,
                        PlaceType = placeType,
                        EnterLimit = enterLimit,
                        BasePoint = basePoint
                       }
    end

    return placeList
end

-- -----------------------------------------------------
-- EnterGamePlaceMsg
--     nKindId      -   INT32
--     nPlaceId     -   INT32
-- -----------------------------------------------------
function packEnterGamePlaceMsg(kindId, placeId)
    local header, headerLen = packNetMsgHeader(MAINMSGID.MSG_MAINID_GAMELIST, SUBMSGID.MSG_SUBID_ENTER_GAMEPLACE)

    local msg = string.pack("Aii", header, kindId, placeId)
    return msg, #msg
end

-- -----------------------------------------------------
-- RoomAddressMsg
--     szIP         -   char[20]
--     sPort        -   UINT16
-- -----------------------------------------------------
function parseRoomAddressMsg(data, len)
    assert(22 == len, "Invalid Room Address Msg!")
    local nextPos, addr, port = string.unpack(data, "A20H")

    return {RoomAddr = addr, RoomPort = port}
end

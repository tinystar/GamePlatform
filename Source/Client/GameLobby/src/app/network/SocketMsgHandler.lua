-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/8/9
-- *********************************************************************

require "app.network.NetMsgGlobalDefs"
require "app.network.SocketMsgMapper"

-- -----------------------------------------------------
-- msg map entries
-- MainMsgId            SubMsgId            MsgHandler
-- -----------------------------------------------------
local mainSvrMsgMap = {
}

local roomSvrMsgMap = {
}

function mapMainServerNetMsg(sockObj)
    SocketMsgMapper.registerMsgHandler(sockObj, mainSvrMsgMap)
end

function unmapMainServerNetMsg(sockObj)
    SocketMsgMapper.removeMsgHandler(sockObj, mainSvrMsgMap)
end

function mapRoomServerNetMsg(sockObj)
    SocketMsgMapper.registerMsgHandler(sockObj, roomSvrMsgMap)
end

function unmapRoomServerNetMsg(sockObj)
    SocketMsgMapper.removeMsgHandler(sockObj, roomSvrMsgMap)
end

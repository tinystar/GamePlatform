-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/8/9
-- *********************************************************************

require "app.network.SocketMsgMapper"

-- -----------------------------------------------------
-- msg map entries
-- MainMsgId            SubMsgId            Handler
-- -----------------------------------------------------
local globalMsgMaps = {
    
}

SocketMsgMapper.registerMsgHandler()
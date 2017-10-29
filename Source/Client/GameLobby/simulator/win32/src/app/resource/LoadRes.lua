-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/10/29
-- *********************************************************************

local CURRENT_MODULE_NAME = ...

cc.exports.gres = {}

local function loadResString()
    local curLangType = cc.Application:getInstance():getCurrentLanguage()

    if cc.LANGUAGE_CHINESE == curLangType then
        gres.str = import(".StringRes_chs", CURRENT_MODULE_NAME)
    else
        gres.str = import(".StringRes_enu", CURRENT_MODULE_NAME)
    end
end

loadResString()

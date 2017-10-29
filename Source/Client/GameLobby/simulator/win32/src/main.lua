
cc.FileUtils:getInstance():setPopupNotify(false)

require "config"
-- load modules that contains global variables before the cocos framework disables global variables
require "GameConfig"
require "app.model.GData"
require "app.network.ClientSocketMgr"
require "app.network.NetMsgGlobalDefs"
require "cocos.init"
require "app.resource.LoadRes"

local function main()
    ClientSocketMgr.init()
    require("app.MyApp"):create():run("LoginScene")
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end

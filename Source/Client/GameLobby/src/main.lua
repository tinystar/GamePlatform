
cc.FileUtils:getInstance():setPopupNotify(false)

require "config"
-- load modules that contains global variables before the cocos framework disables global variables
require "app.model.GData"
require "app.network.ClientSocketMgr"
require "cocos.init"

local function main()
    ClientSocketMgr.init()
    local clientSock = ClientSocketMgr.createSocket("client")
    clientSock:addConnectEventListener(onSocketConnected)
    clientSock:addReceiveEventListener(onSocketReceived)
    clientSock:connect(__GData__.serverAddress, __GData__.serverPort)
    --require("app.MyApp"):create():run()
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end


local ConnectScene = class("ConnectScene", cc.load("mvc").ViewBase)

ConnectScene.RESOURCE_FILENAME = "ConnectScene.csb"
ConnectScene.RESOURCE_BINDING = {
    ["Panel_Connect"] = {["varname"] = "ConnectPanel"}
}

function ConnectScene:onConnectBtnClick(sender)
    local ip = self.TextFieldIp:getString()
    local port = self.TextFieldPort:getString()
    __GData__.frameSock:connect(ip, tonumber(port))
end

function ConnectScene:onCreate()
    local btnConnect = self.ConnectPanel:getChildByName("Button_Connect")
    if btnConnect ~= nil then
        btnConnect:addClickEventListener(handler(self, ConnectScene.onConnectBtnClick))
    end

    self.TextFieldIp = ccui.Helper:seekWidgetByName(self.ConnectPanel, "TextField_IP")
    self.TextFieldPort = ccui.Helper:seekWidgetByName(self.ConnectPanel, "TextField_Port")   
end

function ConnectScene:onSocketConnected(sockObj)
    self:getApp():enterScene("MainScene")
end

function ConnectScene:onEnter()
    local clientSock = ClientSocketMgr.createSocket("client")
    clientSock:addConnectEventListener(handler(self, ConnectScene.onSocketConnected))
    __GData__.frameSock = clientSock
end

return ConnectScene

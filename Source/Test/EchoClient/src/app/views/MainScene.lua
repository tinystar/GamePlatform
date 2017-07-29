
local MainScene = class("MainScene", cc.load("mvc").ViewBase)

MainScene.RESOURCE_FILENAME = "MainScene.csb"

function MainScene:onSendBtnClick(sender)
    local sInput = self.TextFieldInput:getString()
    if #sInput > 0 then
        __GData__.frameSock:sendData(sInput)

        self.BtnSend:setEnabled(false)

        local clientText = self:createEchoItem("Client:", sInput)
        self.PanelEcho:addChild(clientText)
        self.PanelEcho:requestDoLayout()

        self.TextFieldInput:setString("")
    end
end

function MainScene:onSocketMsgReceived(sockObj, data, len)
    local serverText = self:createEchoItem("Server:", data)
    self.PanelEcho:addChild(serverText)
    self.PanelEcho:requestDoLayout()

    self.BtnSend:setEnabled(true)
end

function MainScene:createEchoItem(who, content)
    local item = self.EchoItemModel:clone()
    local textWho = item:getChildByName("Text_Who")
    textWho:setString(who)
    local textContent = item:getChildByName("Text_Content")
    textContent:setString(content)

    return item
end

function MainScene:onTimeServerConnected(sockObj)
    self.PanelTime:setVisible(true)
end

function MainScene:onTimeSockMsgReceived(sockObj, data, len)
    local nextPos, hour, min, sec = string.unpack(data, "III")
    self.TextTime:setString(string.format("%02d:%02d:%02d", hour, min, sec))
end

function MainScene:onCreate()
    __GData__.frameSock:addReceiveEventListener(handler(self, MainScene.onSocketMsgReceived))

    local timeSock = ClientSocketMgr.createSocket("time")
    timeSock:addConnectEventListener(handler(self, MainScene.onTimeServerConnected))
    timeSock:addReceiveEventListener(handler(self, MainScene.onTimeSockMsgReceived))
    timeSock:connect("127.0.0.1", 5015)

    local rootNode = self:getResourceNode()
    self.BtnSend = rootNode:getChildByName("Button_Send")
    self.BtnSend:addClickEventListener(handler(self, MainScene.onSendBtnClick))

    self.TextFieldInput = rootNode:getChildByName("Image_1"):getChildByName("TextField_Input")

    self.PanelEcho = rootNode:getChildByName("ListView_Echo")

    local ItemNode = cc.CSLoader:createNode("EchoItem.csb")
    self.EchoItemModel = ItemNode:getChildByName("Panel_Item")
    self.EchoItemModel:retain()

    self.PanelTime = rootNode:getChildByName("Panel_Time")
    self.TextTime = self.PanelTime:getChildByName("Text_Time")
    self.PanelTime:setVisible(false)
    self.TextTime:setString("")
end

function MainScene:onExit()
    if self.EchoItemModel ~= nil then
        self.EchoItemModel:release()
    end
end

return MainScene

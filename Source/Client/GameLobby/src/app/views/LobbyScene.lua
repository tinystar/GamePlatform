--region *.lua
--Date
--此文件由[BabeLua]插件自动生成


local UserInfoLayer = import(".UserInfoLayer")

local LobbyScene = class("LobbyScene",require("app.views.LobbyViewbase"))



LobbyScene.RESOURCE_FILENAME = "GameLobbyScene.csb"
LobbyScene.RESOURCE_BINDING = {
    ["PanelLobby"] = {["varname"] = "panelLobby"},
    ["PanelLobby.HeadBox.imageHead"] = {["varname"] = "imageHead" ,["events"] = {{["event"] = "touch",["method"] = "OnUserInfoClick" }}},
    ["PanelLobby.btnAddGlod"] = {["varname"] = "btnAddGlod" ,["events"] = {{["event"] = "touch",["method"] = "OnBtnAddGlodClick" }}},
    ["PanelLobby.TextGlod"] = {["varname"] = "textGlod"},
    ["PanelLobby.PaneTop.BtnShop"] = {["varname"] = "btnShop",["events"] = {{["event"] = "touch",["method"] = "OnBtnShopClick" }}},
    ["PanelLobby.PaneTop.BtnExchange"] = {["varname"] = "btnExchange",["events"] = {{["event"] = "touch",["method"] = "OnBtnExchangeClick" }}},
    ["PanelLobby.PaneTop.BtnFriends"] = {["varname"] = "btnFriends",["events"] = {{["event"] = "touch",["method"] = "OnBtnFriendsClick" }}},
    ["PanelLobby.PaneTop.BtnRecord"] = {["varname"] = "btnRecord",["events"] = {{["event"] = "touch",["method"] = "OnBtnRecordClick" }}},
    ["PanelLobby.PaneTop.BtnMessage"] = {["varname"] = "btnMessage",["events"] = {{["event"] = "touch",["method"] = "OnBtnMessageClick" }}},
    ["PanelLobby.PaneTop.BtnSetting"] = {["varname"] = "btnSetting",["events"] = {{["event"] = "touch",["method"] = "OnBtnSettingClick" }}},
    ["PanelLobby.PanelLeft.BtnRankList"] = {["varname"] = "btnRankList",["events"] = {{["event"] = "touch",["method"] = "OnBtnRankListClick" }}},
    ["PanelLobby.PanelLeft.BtnDailyTask"] = {["varname"] = "btnDailyTask",["events"] = {{["event"] = "touch",["method"] = "OnBtnDailyTaskClick" }}},
    ["PanelLobby.PanelLeft.BtnLongTask"] = {["varname"] = "btnLongTask",["events"] = {{["event"] = "touch",["method"] = "OnBtnLongTaskClick" }}},
    ["PanelLobby.PageViewGameList.Panel_Page1.BtnRoomType1"] = {["varname"] = "btnRoomType1",["events"] = {{["event"] = "touch",["method"] = "OnBtnRoomType1Click" }}},
    ["PanelLobby.PageViewGameList.Panel_Page1.BtnRoomType2"] = {["varname"] = "btnRoomType2",["events"] = {{["event"] = "touch",["method"] = "OnBtnRoomType2Click" }}},
    ["PanelLobby.PageViewGameList.Panel_Page1.BtnRoomType3"] = {["varname"] = "btnRoomType3",["events"] = {{["event"] = "touch",["method"] = "OnBtnRoomType3Click" }}},
    ["PanelLobby.PageViewGameList.Panel_Page1.BtnRoomType4"] = {["varname"] = "btnRoomType4",["events"] = {{["event"] = "touch",["method"] = "OnBtnRoomType4Click" }}},
}

function LobbyScene:onCreate()
   print("onCreate")     
end


--个人信息--
function LobbyScene:OnUserInfoClick(sender)
    print("OnUserInfoClick")
    local userInfoLayer = UserInfoLayer:create()
    self:addChild(userInfoLayer)
end

--加金币按钮--
function LobbyScene:OnBtnAddGlodClick(sender)
    print("OnBtnAddGlodClick")
end

--商店--
function LobbyScene:OnBtnShopClick(sender)
    print("OnBtnShopClick")
end

--兑换--
function LobbyScene:OnBtnExchangeClick(sender)
    print("OnBtnExchangeClick")
end

--好友--
function LobbyScene:OnBtnFriendsClick(sender)
    print("OnBtnFriendsClick")
end

--战绩--
function LobbyScene:OnBtnRecordClick(sender)
    print("OnOnBtnRecordClick")
end

--消息--
function LobbyScene:OnBtnMessageClick(sender)
    print("OnBtnMessageClickClick")
end

--设置--
function LobbyScene:OnBtnSettingClick(sender)
    print("OnBtnSettingClick")
end

--排行榜--
function LobbyScene:OnBtnRankListClick(sender)
    print("OnBtnRankListClick")
end

--日常任务--
function LobbyScene:OnBtnDailyTaskClick(sender)
    print("OnBtnDailyTaskClick")
end

--成就任务--
function LobbyScene:OnBtnLongTaskClick(sender)
    print("OnBtnLongTaskClick")
end

--晃晃麻将--
function LobbyScene:OnBtnRoomType1Click(sender)
    print("OnBtnRoomType1Click")
end

--好友约战--
function LobbyScene:OnBtnRoomType2Click(sender)
    print("OnBtnRoomType2Click")
end

--比赛场--
function LobbyScene:OnBtnRoomType3Click(sender)
    print("OnBtnRoomType3Click")
end

--召唤朋友--
function LobbyScene:OnBtnRoomType4Click(sender)
    print("OnBtnRoomType4Click")
end

return LobbyScene


--endregion

--region *.lua
--Date
--此文件由[BabeLua]插件自动生成


local UserInfoLayer = class("UserInfoLayer", require("app.views.LobbyPopupLayer"))


UserInfoLayer.RESOURCE_LAYER_NAME = "UserInfoLayer.csb"
UserInfoLayer.RESOURCE_LAYER_BINDING = {
    ["PanelContent.PanelLeft.BtnUserInfo"] = {["varname"] = "btnUserInfo" ,["events"] = {{["event"] = "touch",["method"] = "OnUserInfoClick" }}},
    ["PanelContent.PanelLeft.BtnPerfectInfo"] = {["varname"] = "btnPerfectInfo" ,["events"] = {{["event"] = "touch",["method"] = "OnPerfectInfoClick" }}},
    ["BtnClose"] = {["varname"] = "btnClose" ,["events"] = {{["event"] = "touch",["method"] = "OnCloseClick" }}},
 }

function UserInfoLayer:OnUserInfoClick()
    print("UserInfoLayer:OnUserInfoClick")
end

function UserInfoLayer:OnPerfectInfoClick()
    print("UserInfoLayer:OnPerfectInfoClick")
end

function UserInfoLayer:OnCloseClick()
    print("UserInfoLayer:OnCloseClick")
    self:removeFromParent()
end

return UserInfoLayer
--endregion

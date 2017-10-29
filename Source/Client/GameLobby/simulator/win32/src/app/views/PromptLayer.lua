-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/10/27
-- *********************************************************************

local PromptLayer = class("PromptLayer", cc.LayerColor)

function PromptLayer:ctor(prompt)
    self.promptMsg_ = prompt

    self:onCreate()
end

function PromptLayer:onCreate()
    self:initWithColor(cc.c4b(0,0,0,225))

    local rootNode = cc.CSLoader:createNodeWithVisibleSize("PromptLayer.csb")
    self:addChild(rootNode)

    local bgNode = rootNode:getChildByName("bg")
    local promptTxt = bgNode:getChildByName("Text_Prompt")
    local okBtn = bgNode:getChildByName("Button_OK")
    promptTxt:setString(self.promptMsg_)
    okBtn:addClickEventListener(function(sender)
        self:removeFromParent()
    end)
end

return PromptLayer

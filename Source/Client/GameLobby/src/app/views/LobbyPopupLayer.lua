--region *.lua
--Date
--此文件由[BabeLua]插件自动生成


local PopupLayer = class("PopupLayer", cc.LayerColor)

function PopupLayer:ctor()
    self:enableNodeEvents()
    local res = rawget(self.class, "RESOURCE_LAYER_NAME")
    if res then
        self:createResourceLayerNode(res)
    end

    local binding = rawget(self.class, "RESOURCE_LAYER_BINDING")
    if res and binding then
        self:createResourceLayerBinding(binding)
    end

    self:onCreate()
end


function PopupLayer:createResourceLayerNode(resourceFilename)
    if self.resourceNode_ then
        self.resourceNode_:removeSelf()
        self.resourceNode_ = nil
    end
    self.resourceNode_ = cc.CSLoader:createNodeWithVisibleSize(resourceFilename)
    assert(self.resourceNode_, string.format("PopupLayer:createResourceLayerNode() - load resouce node from file \"%s\" failed", resourceFilename))
    self:addChild(self.resourceNode_)
end

function PopupLayer:createResourceLayerBinding(binding)
    assert(self.resourceNode_, "LobbyViewBase:createResourceBinding() - not load resource node")
    for nodeName, nodeBinding in pairs(binding) do
        local node = self:getLayerChildByName(nodeName)
        if not node then
         print(string.format("nodeName error: %s ", nodeName))
         assert(node, "LobbyViewBase:createResourceBinding() - nodeName error")
        end
        if nodeBinding.varname then
            self[nodeBinding.varname] = node
        end
        for _, event in ipairs(nodeBinding.events or {}) do
            if event.event == "touch" then
                node:onTouch(handler(self, self[event.method]))
            end
        end
    end
end

function PopupLayer:getLayerChildByName(nodeFullName)
  local nodeResNode = self.resourceNode_
  local curFullNodeName = nodeFullName;
  local strArr = string.split(curFullNodeName,'.')
  if not strArr  then
    return nil;
   end
   for i,str in ipairs(strArr) do
        assert(nodeResNode, string.format("LobbyViewBase:getChildByName - not find  %s resource node",nodeFullName))
        nodeResNode = nodeResNode:getChildByName(tostring(str))
   end
   return nodeResNode
end

function PopupLayer:onCreate()

end

return PopupLayer


--endregion

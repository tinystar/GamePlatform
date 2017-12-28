--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

local LobbyViewBase = class("LobbyViewBase", cc.load("mvc").ViewBase)

function LobbyViewBase:createResourceBinding(binding)
    assert(self.resourceNode_, "LobbyViewBase:createResourceBinding() - not load resource node")
    for nodeName, nodeBinding in pairs(binding) do
        local node = self:getChildByName(nodeName)
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

function LobbyViewBase:getChildByName(nodeFullName)
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

return LobbyViewBase

--endregion

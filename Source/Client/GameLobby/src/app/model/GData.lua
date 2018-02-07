-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/7/16
-- *********************************************************************

-- global game context data
__GData__ = {
    MainSvrAddress = nil,
    MainSvrPort = nil,
    MainSocket = nil,

    GameUser = {
        UserId = nil,
        Account = nil,
        UserName = nil,
        HeadIndex = nil,
        Gender = nil,
        Money = nil,
        RoomCard = nil,
        PhoneNumber = nil,
        TypeFlag = nil
    },

    -- ��Ϸ�����б�Ϊһ������
    -- ÿһ��Ԫ������Ϊ��
    --     {
    --       KindId,
    --       GameName,
    --       ClientMod,         -- �ͻ�����Ϸ�����ļ�
    --       GameVer,
    --       PlaceList =        -- �����б�ÿ��Ԫ������Ϊ��
    --                   {
    --                      PlaceId,
    --                      PlaceName,
    --                      PlaceType,
    --                      EnterLimit,
    --                      BasePoint,
    --                   }
    --     }
    GameList = {},
}

return __GData__

-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/10/31
-- *********************************************************************

CustEvents = {
    PromptAdd       = "Prompt.Add",
    PromptRemove    = "Prompt.Remove",
    ConnMainSucc    = "Connect.Main.Success",
    ConnMainFail    = "Connect.Main.Failed",
    MainSockClose   = "Main.Socket.Close",
    ConnRoomSucc    = "Connect.Room.Success",
    ConnRoomFail    = "Connect.Room.Failed",
    RoomSockClose   = "Room.Socket.Close"
}

GenderType = {
    Male            = 0,
    Female          = 1
}

UserHeadIndex = {
    HeadCustom      = -1,
    HeadDefault     = 0
}

PreDefHeadImg = {
    [0]             = "head/head0.png",
    [1]             = "head/head1.png",
    [2]             = "head/head2.png",
    [3]             = "head/head3.png",
    [4]             = "head/head4.png",
    [5]             = "head/head5.png",
    [6]             = "head/head6.png",
    [7]             = "head/head7.png",
    [8]             = "head/head8.png"
}

LoginFailReason = {
    UnknownReason   = 0,
    AccountNotExist = 1,
    PasswordWrong   = 2,
    GuestCreateFail = 3,
    AlreadyLogin    = 4
}

##########################################################################
# All rights reserved.
# You can't use, copy or modify the file without the permission of the
# author. Otherwise I will be held accountable under the law.
#
# Author: xiaoxin
# Contact: 765392700(QQ)
# Date: 2018/1/17
##########################################################################

USE xCardGame;

# add games data
INSERT INTO gamekind(kindid, gamename, clientmodule, servermodule, version, sortfield)
VALUES(1, "经典斗地主", "landlord.lua", "landlord.dll", "1.0.0", 0);
INSERT INTO gamekind(kindid, gamename, clientmodule, servermodule, version, sortfield)
VALUES(2, "诈金花", "zjh.lua", "zjh.dll", "1.0.0", 0);
INSERT INTO gamekind(kindid, gamename, clientmodule, servermodule, version, sortfield)
VALUES(3, "武汉麻将", "whmj.lua", "whmj.dll", "1.0.0", 0);
INSERT INTO gamekind(kindid, gamename, clientmodule, servermodule, version, sortfield)
VALUES(4, "捕鱼", "FishingJoy.lua", "FishingJoy.dll", "1.0.0", 0);

# 经典斗地主
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(1, 1, "初级场", 0, 100.00, 10.00);
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(1, 2, "中级场", 0, 1000.00, 100.00);
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(1, 3, "高级场", 0, 10000.00, 1000.00);

# 诈金花
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(2, 1, "初级场", 0, 500.00, 100.00);

# 武汉麻将
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(3, 1, "好友场", 1, 100.00, 10.00);
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(3, 2, "初级场", 0, 100.00, 10.00);
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(3, 3, "中级场", 1, 1000.00, 100.00);

# 捕鱼
INSERT INTO gameplace(kindid, placeid, placename, placetype, enterlimit, basepoint)
VALUES(4, 1, "初级场", 1, 1000.00, 10.00);

# 经典斗地主-初级场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport, maxuser)
VALUES(1, 1, 1, "房间1", "127.0.0.1", 8001, 1200);
# 经典斗地主-中级场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(1, 2, 1, "房间2", "127.0.0.1", 8002);
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(1, 2, 2, "房间3", "127.0.0.1", 8003);
# 经典斗地主-中级场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(1, 3, 1, "房间4", "127.0.0.1", 8004);

# 诈金花-初级场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(2, 1, 1, "房间1", "127.0.0.1", 8031);
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(2, 1, 2, "房间2", "127.0.0.1", 8032);

# 武汉麻将-好友场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(3, 1, 1, "房间1", "127.0.0.1", 8051);
# 武汉麻将-初级场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(3, 2, 1, "房间2", "127.0.0.1", 8052);
# 武汉麻将-中级场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(3, 3, 1, "房间3", "127.0.0.1", 8053);

# 捕鱼-初级场
INSERT INTO gameroom(kindid, placeid, roomid, roomname, serverip, serverport)
VALUES(4, 1, 1, "房间1", "127.0.0.1", 8071);
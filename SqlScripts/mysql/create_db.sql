##########################################################################
# All rights reserved.
# You can't use, copy or modify the file without the permission of the
# author. Otherwise I will be held accountable under the law.
#
# Author: xiaoxin
# Contact: 765392700(QQ)
# Date: 2017/12/10
##########################################################################

# create card game database
CREATE DATABASE IF NOT EXISTS xBoardGame
    CHARACTER SET = utf8;

USE xBoardGame;

# create TUserInfo table
CREATE TABLE IF NOT EXISTS TUserInfo
(
    userid        int               NOT NULL AUTO_INCREMENT,
    account       char(16)          NOT NULL UNIQUE,
    password      char(16)          NOT NULL,
    username      varchar(64)       NOT NULL,
    headIndex     int               NULL DEFAULT 0,
    gender        boolean           NULL DEFAULT 0,
    money         decimal(10,2)     NULL DEFAULT 0,
    roomcardcount int unsigned      NULL DEFAULT 0,
    phonenumber   char(15)          NULL,
    flag          int unsigned      NULL DEFAULT 0,
    PRIMARY KEY(userid)
)ENGINE = InnoDB, AUTO_INCREMENT = 100001;

# create game kind table
CREATE TABLE IF NOT EXISTS TGameKind
(
    kindid        int               NOT NULL AUTO_INCREMENT,
    gamename      varchar(23)       NOT NULL UNIQUE,
    clientmodule  varchar(23)       NOT NULL,
    servermodule  varchar(23)       NULL,
    version       char(15)          NOT NULL,
    sortfield     int               NULL DEFAULT 0,
    PRIMARY KEY(kindid)
)ENGINE = InnoDB;

# create game place table
CREATE TABLE IF NOT EXISTS TGamePlace
(
    kindid        int               NOT NULL,
    placeid       int               NOT NULL,
    placename     varchar(23)       NOT NULL,
    placetype     int               NOT NULL,
    enterlimit    decimal(10,2)     NOT NULL,
    basepoint     decimal(10,2)     NOT NULL,
    PRIMARY KEY(kindid, placeid)
)ENGINE = InnoDB;

# create game room table
CREATE TABLE IF NOT EXISTS TGameRoom
(
    kindid        int               NOT NULL,
    placeid       int               NOT NULL,
    roomid        int               NOT NULL,
    roomname      varchar(23)       NULL,
    serverip      char(19)          NOT NULL,
    serverport    smallint unsigned NOT NULL,
    maxuser       int unsigned      NOT NULL DEFAULT 300,
    PRIMARY KEY(kindid, placeid, roomid)
)ENGINE = InnoDB;

# add foreign keys
ALTER TABLE TGamePlace
ADD CONSTRAINT fk_gameplace_kind
FOREIGN KEY(kindid) REFERENCES TGameKind(kindid)
ON UPDATE CASCADE
ON DELETE CASCADE;

ALTER TABLE TGameRoom
ADD CONSTRAINT fk_gameroom_place
FOREIGN KEY(kindid, placeid) REFERENCES TGamePlace(kindid, placeid)
ON UPDATE CASCADE
ON DELETE CASCADE;


# create main server login record table
CREATE TABLE IF NOT EXISTS TMainLoginRecord
(
    userid        int               NOT NULL,
    servername    varchar(64)       NULL,
    loginip       char(19)          NOT NULL,
    os            varchar(24)       NULL,
    device        varchar(64)       NULL,
    logintime     datetime          NOT NULL,
    PRIMARY KEY(userid)
)ENGINE = InnoDB;

# add foreign keys
ALTER TABLE TMainLoginRecord
ADD CONSTRAINT fk_mainlogin_user
FOREIGN KEY(userid) REFERENCES TUserInfo(userid);

# create main server login history table
CREATE TABLE IF NOT EXISTS TMainLoginHistory
(
    id            int               NOT NULL AUTO_INCREMENT,
    userid        int               NOT NULL,
    servername    varchar(64)       NULL,
    loginip       char(19)          NOT NULL,
    os            varchar(24)       NULL,
    device        varchar(64)       NULL,
    logintime     datetime          NOT NULL,
    logouttime    datetime          NOT NULL,
    PRIMARY KEY(id)
)ENGINE = InnoDB;

# create room server login record table
CREATE TABLE IF NOT EXISTS TRoomLoginRecord
(
    userid        int               NOT NULL,
    kindid        int               NOT NULL,
    placeid       int               NOT NULL,
    roomid        int               NOT NULL,
    loginip       char(19)          NOT NULL,
    logintime     datetime          NOT NULL,
    PRIMARY KEY(userid)
)ENGINE = InnoDB;

# add foreign keys
ALTER TABLE TRoomLoginRecord
ADD CONSTRAINT fk_roomlogin_user
FOREIGN KEY(userid) REFERENCES TUserInfo(userid);

ALTER TABLE TRoomLoginRecord
ADD CONSTRAINT fk_roomlogin_room
FOREIGN KEY(kindid, placeid, roomid) REFERENCES TGameRoom(kindid, placeid, roomid)
ON UPDATE CASCADE
ON DELETE CASCADE;

# create room server login history table
CREATE TABLE IF NOT EXISTS TRoomLoginHistory
(
    id            int               NOT NULL AUTO_INCREMENT,
    userid        int               NOT NULL,
    kindid        int               NOT NULL,
    placeid       int               NOT NULL,
    roomid        int               NOT NULL,
    loginip       char(19)          NOT NULL,
    logintime     datetime          NOT NULL,
    logouttime    datetime          NOT NULL,
    PRIMARY KEY(id)
)ENGINE = InnoDB;


# create user
DROP USER dbserver;
CREATE USER dbserver IDENTIFIED BY 'xx.1314.z';
GRANT ALL ON xBoardGame.* TO dbserver;

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
CREATE DATABASE IF NOT EXISTS xCardGame
    CHARACTER SET = utf8;

USE xCardGame;

# create userinfo table
CREATE TABLE IF NOT EXISTS userinfo
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
CREATE TABLE IF NOT EXISTS gamekind
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
CREATE TABLE IF NOT EXISTS gameplace
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
CREATE TABLE IF NOT EXISTS gameroom
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
ALTER TABLE gameplace
ADD CONSTRAINT fk_gameplace_kind
FOREIGN KEY(kindid) REFERENCES gamekind(kindid)
ON UPDATE CASCADE
ON DELETE CASCADE;

ALTER TABLE gameroom
ADD CONSTRAINT fk_gameroom_place
FOREIGN KEY(kindid, placeid) REFERENCES gameplace(kindid, placeid)
ON UPDATE CASCADE
ON DELETE CASCADE;


# create user
CREATE USER dbserver IDENTIFIED BY 'xx.1314.z';
GRANT ALL ON xCardGame.* TO dbserver;

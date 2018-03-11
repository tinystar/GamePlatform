##########################################################################
# All rights reserved.
# You can't use, copy or modify the file without the permission of the
# author. Otherwise I will be held accountable under the law.
#
# Author: xiaoxin
# Contact: 765392700(QQ)
# Date: 2017/12/31
##########################################################################

##########################################################################
# Create a new guest account
# Parameters:
#   [IN]    pusername: user name prefix
#   [IN]    ppassword:
#   [IN]    pmoney: initial money when the guest account first created
#   [IN]    pRmCardCnt: initial room card count when the guest account first created
#   [IN]    pflag: initial UserType mask(kGuest and so on)
#   [OUT]   paccount: account that be created
##########################################################################
DROP PROCEDURE IF EXISTS create_guest_account;
DELIMITER ;;
CREATE PROCEDURE create_guest_account(
    IN  pusername   varchar(255),
    IN  ppassword   char(16),
    IN  pheadIdx    int,
    IN  pgender     boolean,
    IN  pmoney      decimal(10,2),
    IN  pRmCardCnt  int unsigned,
    IN  pflag       int unsigned,
    OUT paccount    char(16)
)
BEGIN
    DECLARE maxAcct char(16);
    DECLARE acctName char(16);
    DECLARE acctNumb char(16);
    DECLARE usrName varchar(255);
    DECLARE maxNum int unsigned;
    
    # 查询号码最大的游客帐号（帐号规则：Guest + 4位数字）
	SELECT account INTO maxAcct
    FROM TUserInfo
    WHERE account REGEXP 'Guest[[:digit:]]{4}'
    ORDER BY account DESC
    LIMIT 1;
    
    IF ISNULL(maxAcct) || LENGTH(trim(maxAcct)) < 1 THEN
		SET acctName = 'Guest0001';
		SELECT CONCAT(pusername, '0001') INTO usrName;
	ELSE
		SELECT CAST(RIGHT(TRIM(maxAcct), 4) AS unsigned) INTO maxNum;
        SET maxNum = maxNum + 1;
        SELECT LPAD(CAST(maxNum AS char), 4, '0') INTO acctNumb;
        SELECT CONCAT('Guest', acctNumb) INTO acctName;
        SELECT CONCAT(pusername, acctNumb) INTO usrName;
    END IF;
    
    INSERT INTO TUserInfo(
		account,
		password,
		username,
		headIndex,
		gender,
		money,
		roomcardcount,
		flag)
	VALUES(
		acctName,
		ppassword,
		usrName,
		pheadIdx,
		pgender,
		pmoney,
		pRmCardCnt,
		pflag);
        
	IF ROW_COUNT() = 0 THEN
		SET paccount = '';
	ELSE
		SET paccount = acctName;
	END IF;
END;;       # end create_guest_account
DELIMITER ;


##########################################################################
# Validate user login main server
# Parameters:
#   [IN]    puserid: user id
#   [IN]    ppassword: input password
#   [IN]    pservername: main server name
#   [IN]    ploginip: user login ip address
#   [IN]    pos: operation system
#   [IN]    pdevice: device type
#   [OUT]   presult: output login result
#                       -- 0  登录成功
#                       -- 1  用户不存在
#                       -- 2  已经登录
#                       -- 3  密码不正确
#                       -- 4  内部错误
##########################################################################
DROP PROCEDURE IF EXISTS validate_main_login_by_userid;
DELIMITER ;;
CREATE PROCEDURE validate_main_login_by_userid(
    IN  puserid     int,
    IN  ppassword   char(16),
    IN  pservername varchar(64),
    IN  ploginip    char(19),
    IN  pos         varchar(24),
    IN  pdevice     varchar(64),
    OUT presult     int
)
label_pro: BEGIN
    DECLARE bExist int;
    DECLARE varPassword char(16);
    
    # 查询用户是否存在
    SELECT count(*) INTO bExist
    FROM TUserInfo
    WHERE userid = puserid;
    
    IF bExist = 0 THEN      # 用户不存在
        SET presult = 1;
        LEAVE label_pro;
    END IF;
    
    # 查询是否已经登录，防止重复登录
    SELECT count(*) INTO bExist
    FROM TMainLoginRecord
    WHERE userid = puserid;
    
    IF bExist = 1 THEN      # 已经登录
        SET presult = 2;
        LEAVE label_pro;
    END IF;
    
    # 获取密码
    SELECT password INTO varPassword
    FROM TUserInfo
    WHERE userid = puserid;
    
    IF ppassword <> varPassword THEN    # 密码不正确
        SET presult = 3;
        LEAVE label_pro;
    END IF;
    
    INSERT INTO TMainLoginRecord(
		userid,
		servername,
		loginip,
		os,
		device,
		logintime)
	VALUES(
		puserid,
		pservername,
		ploginip,
		pos,
		pdevice,
		now());
		
	IF ROW_COUNT() = 0 THEN
		SET presult = 4;        # 内部错误
		LEAVE label_pro;
	END IF;
	
	# 登录成功
	SET presult = 0;
END;;       # end validate_main_login_by_userid
DELIMITER ;


##########################################################################
# Delete main login record when user logout from main server
# Parameters:
#   [IN]    puserid: user id
##########################################################################
DROP PROCEDURE IF EXISTS delete_main_login_record;
DELIMITER ;;
CREATE PROCEDURE delete_main_login_record(
    IN  puserid     int
)
BEGIN
	DELETE FROM  TMainLoginRecord
	WHERE userid = puserid;
END;;       # end delete_main_login_record
DELIMITER ;
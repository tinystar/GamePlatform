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
#   [IN]    pusername:user name prefix
#   [IN]    ppassword:
#   [IN]    pmoney:initial money when the guest account first created
#   [IN]    pRmCardCnt:initial room card count when the guest account first created
#   [IN]    pflag:initial UserType mask(kGuest and so on)
#   [OUT]   paccount:account that be created
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
    FROM userinfo
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
    
    INSERT INTO userinfo(
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

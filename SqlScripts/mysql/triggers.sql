##########################################################################
# All rights reserved.
# You can't use, copy or modify the file without the permission of the
# author. Otherwise I will be held accountable under the law.
#
# Author: xiaoxin
# Contact: 765392700(QQ)
# Date: 2018/3/7
##########################################################################

##########################################################################
# Backup main server login record to history when deleted
# Note: We create after delete trigger rather than before trigger, because
#       we need delete main login record even if backup failed.
##########################################################################
DROP TRIGGER IF EXISTS after_delete_main_login_record;
DELIMITER ;;
CREATE TRIGGER after_delete_main_login_record AFTER DELETE ON TMainLoginRecord
FOR EACH ROW
BEGIN
    INSERT INTO TMainLoginHistory(
	    userid,
	    servername,
	    loginip,
	    os,
	    device,
	    logintime,
	    logouttime)
    VALUES(
	    OLD.userid,
	    OLD.servername,
	    OLD.loginip,
	    OLD.os,
	    OLD.device,
	    OLD.logintime,
	    now());
END;;
DELIMITER ;
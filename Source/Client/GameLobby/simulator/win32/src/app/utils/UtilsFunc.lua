-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/11/8
-- *********************************************************************

function paddingWith(data, len, paddingVal)
    paddingVal = paddingVal or '\0'

    if #data >= len then
        return data
    else
        local paddings = string.rep(paddingVal, len - #data)
        return data .. paddings
    end
end

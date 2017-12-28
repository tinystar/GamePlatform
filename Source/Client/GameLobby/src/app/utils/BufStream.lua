-- *********************************************************************
-- All rights reserved.
-- You can't use, copy or modify the file without the permission of the
-- author. Otherwise I will be held accountable under the law.
-- 
-- Author: xiaoxin
-- Contact: 765392700(QQ)
-- Date: 2017/11/7
-- *********************************************************************

require "app.utils.UtilsFunc"

-- Low efficiency, avoid using
local BufStream = class("BufStream")

function BufStream:ctor()
    self.buffer_ = ""
end

function BufStream:ctor(buf)
    self.buffer_ = buf or ""
end

function BufStream:buffer()
    return self.buffer_
end

function BufStream:readByte()
    if #self.buffer_ >= 1 then
        local nextPos, data = string.unpack(self.buffer_, "b")
        self.buffer_ = string.sub(self.buffer_, nextPos)
        return data
    else
        return nil
    end
end

function BufStream:writeByte(data)
    self.buffer_ = self.buffer_ .. string.pack("b", data)
end

function BufStream:readChar()
    if #self.buffer_ >= 1 then
        local nextPos, data = string.unpack(self.buffer_, "c")
        self.buffer_ = string.sub(self.buffer_, nextPos)
        return data
    else
        return nil
    end
end

function BufStream:writeChar(data)
    self.buffer_ = self.buffer_ .. string.pack("c", data)
end

function BufStream:readInt16()
    if #self.buffer_ >= 2 then
        local nextPos, data = string.unpack(self.buffer_, "h")
        self.buffer_ = string.sub(self.buffer_, nextPos)
        return data
    else
        return nil
    end
end

function BufStream:writeInt16(data)
    self.buffer_ = self.buffer_ .. string.pack("h", data)
end

function BufStream:readUInt16()
    if #self.buffer_ >= 2 then
        local nextPos, data = string.unpack(self.buffer_, "H")
        self.buffer_ = string.sub(self.buffer_, nextPos)
        return data
    else
        return nil
    end
end

function BufStream:writeUInt16(data)
    self.buffer_ = self.buffer_ .. string.pack("H", data)
end

function BufStream:readInt32()
    if #self.buffer_ >= 4 then
        local nextPos, data = string.unpack(self.buffer_, "i")
        self.buffer_ = string.sub(self.buffer_, nextPos)
        return data
    else
        return nil
    end
end

function BufStream:writeInt32(data)
    self.buffer_ = self.buffer_ .. string.pack("i", data)
end

function BufStream:readUInt32()
    if #self.buffer_ >= 4 then
        local nextPos, data = string.unpack(self.buffer_, "I")
        self.buffer_ = string.sub(self.buffer_, nextPos)
        return data
    else
        return nil
    end
end

function BufStream:writeUInt32(data)
    self.buffer_ = self.buffer_ .. string.pack("I", data)
end

function BufStream:readBytes(len)
    assert(type(len) == "number", "len must be a number!")
    if #self.buffer_ >= len then
        local nextPos, data = string.unpack(self.buffer_, "A" .. len)
        self.buffer_ = string.sub(self.buffer_, nextPos)
        return data
    else
        return nil
    end
end

function BufStream:writeBytes(data, len, padding)
    self.buffer_ = self.buffer_ .. string.pack("A", paddingWith(data, len, padding))
end

return BufStream

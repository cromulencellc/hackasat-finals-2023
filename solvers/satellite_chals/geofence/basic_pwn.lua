local socket = require("socket")
local udp = socket.udp()
udp:setpeername("localhost", 9001)

-- do a gps script injection that creates a user frame
udp:send(string.char(0x01))
udp:send("a once\r\nuserdatum 6378206.0 294.97 0.0 0.0 0.0 0.0 0.0 3.14 0.0\r\ndatum user\r\nbestpos a")

-- do a gps script injection that uses the frame we setup
udp:send(string.char(0x01))
udp:send("a once\r\ndatum user\r\nbestpos a")

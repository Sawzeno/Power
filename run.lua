local lfs = require("lfs")

local current_dir = lfs.currentdir()

local function execute_command(command)
    print("Running: " .. command)
    local success, exit_code = os.execute(command)
    if not success then
        if type(exit_code) == "number" then
            error("Command failed with exit code: " .. exit_code)
        else
            error("Error executing command: " .. command)
        end
    end
end

local path_separator = package.config:sub(1, 1) -- Get the path separator based on the current OS
local build_dir = current_dir .. path_separator .. "build"
local cmake_cmd = "cmake .. -G"

local os_name = arg[1] and arg[1]:lower() or ""
if os_name == "vs" then
    cmake_cmd = cmake_cmd .. " \"Visual Studio 17 2017\""
else
    cmake_cmd = cmake_cmd .. " \"Unix Makefiles\""
end

if not lfs.chdir(build_dir) then
    lfs.mkdir(build_dir)
end

execute_command("cd " .. build_dir .. " && " .. cmake_cmd)

local build_command
if os_name == "vs" then
    build_command = "cd " .. build_dir .. " && msbuild /p:Configuration=Debug"
else
    build_command = "cd " .. build_dir .. " && make"
end

execute_command(build_command)

local power_dir = build_dir .. path_separator .. "power"
lfs.chdir(power_dir)

local exe_name = "main"
if os_name == "vs" then
    exe_name = exe_name .. ".exe"
end

local success, err_msg = pcall(execute_command, exe_name)
if not success then
    print("Execution interrupted by user.")
end

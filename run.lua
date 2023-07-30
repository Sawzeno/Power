function exec(command)
	print("executing command: " ..command)
	
	local status = os.execute(command)
	
	if status then
		print("+1" ..command)
	else
		print("-1" ..command)
		os.exit(1)
	end
end	
	
local build_dir = "build"
local exists = os.execute("cd "..build_dir)
	
if not exists then
	exec("mkdir "..build_dir.."&& cd "..build_dir.." && cmake ..")
else	
	exec("cd ".. build_dir.." && make")
end	
	
exec("./"..build_dir.."/power")



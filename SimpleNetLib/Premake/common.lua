outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root .. "Bin/")
dirs["temp"]			= os.realpath(dirs.root .. "Temp/")
dirs["lib"]				= os.realpath(dirs.root .. "Lib/")
dirs["projectfiles"]	= os.realpath(dirs.root .. "Local/")
dirs["dependencies"]	= os.realpath(dirs.root .. "Dependencies/")
dirs["simple_net_lib"]	= os.realpath(dirs.root .. "SimpleNetLib/")
dirs["unit_testing"]	= os.realpath(dirs.root .. "UnitTesting/")

if not os.isdir (dirs.bin) then
	os.mkdir (dirs.bin)
end

if not os.isdir (dirs.lib) then
	os.mkdir (dirs.lib)
end
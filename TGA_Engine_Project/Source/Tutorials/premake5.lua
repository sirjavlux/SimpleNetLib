include "../../Premake/extensions.lua"
include "../../Premake/common.lua"

tutorials = os.matchdirs("Tutorial-*")

newoption {
    trigger = "defaultproject",
    value = "Tutorial-01_Init",
    default = "Tutorial-01_Init",
    description = "Select default project to build and run. Mainly useful when generating the tutorials"
}

workspace "Tutorial"
	architecture "x64"
	location "../../"
	
	startproject (_OPTIONS["defaultproject"])

	configurations {
		"Debug",
		"Release",
		"Retail"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include (dirs.external)
include (dirs.engine)

group ("Tutorials") -- creates a filter where all tutorial projects are grouped

for _, v in pairs(tutorials) do
	include (v)
end

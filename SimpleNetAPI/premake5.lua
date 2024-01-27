include "Premake/extensions.lua"
include "Premake/common.lua"

workspace "SimpleNetLib"
	architecture "x86"
	location ""
	
	startproject (_OPTIONS["defaultproject"])

	configurations {
		"Debug",
		"Release",
		"Retail"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include (dirs.simple_net_lib)
include (dirs.unit_testing)
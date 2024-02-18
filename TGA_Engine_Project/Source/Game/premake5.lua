include "../../Premake/extensions.lua"

workspace "Game"
	location "../../"
	startproject "Game"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Retail"
	}

-- include for common stuff 
include "../../Premake/common.lua"

include (dirs.enhetstest)
include (dirs.client)
include (dirs.server)
include (dirs.external)
include (dirs.engine)


-------------------------------------------------------------
project "Game"
	location (dirs.projectfiles)
	dependson { "External", "Engine" }
		
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}"
	targetdir ("%{dirs.bin}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	links {"External", "Engine", "ws2_32.lib", "SimpleNetLib_%{cfg.buildcfg}.lib"}

	includedirs { dirs.external, dirs.engine, dirs.simplenetlib }

	files {
		"source/**.h",
		"source/**.cpp",
		"source/**.hpp",
	}

	libdirs { dirs.lib, dirs.dependencies, dirs.simplenetlib_lib }

	verify_or_create_settings("Game")
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		files {"tools/**"}
		includedirs {"tools/"}
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		files {"tools/**"}
		includedirs {"tools/"}
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"

	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
		--	"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		
		defines {
			"WIN32",
			"_LIB", 
			"TGE_SYSTEM_WINDOWS" 
		}
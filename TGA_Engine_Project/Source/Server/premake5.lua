include "../../Premake/extensions.lua"

-- include for common stuff 
include "../../Premake/common.lua"

include (dirs.external)
include (dirs.engine)
include (dirs.game)

-------------------------------------------------------------
project "Server"
	location (dirs.projectfiles)
	dependson { "External", "Engine", "Game" }
		
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}"
	targetdir ("%{dirs.bin}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	
	links {"External", "Engine", "Game", "ws2_32.lib", "SimpleNetLib_%{cfg.buildcfg}.lib"}

	includedirs { dirs.external, dirs.engine, dirs.game, dirs.source, dirs.simplenetlib }

	files {
		"source/**.h",
		"source/**.cpp",
	}

	libdirs { dirs.lib, dirs.dependencies, dirs.simplenetlib_lib }

	verify_or_create_settings("Server")
	
	filter "configurations:Debug"
		defines {"_DEBUG", "_ITERATOR_DEBUG_LEVEL=2"}
		runtime "Debug"
		symbols "on"
		files {"tools/**"}
		includedirs {"tools/"}
	filter "configurations:Release"
		defines {"_RELEASE", "_ITERATOR_DEBUG_LEVEL=0"}
		runtime "Release"
		optimize "on"
		files {"tools/**"}
		includedirs {"tools/"}
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"

	filter "system:windows"
--		kind "StaticLib"
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
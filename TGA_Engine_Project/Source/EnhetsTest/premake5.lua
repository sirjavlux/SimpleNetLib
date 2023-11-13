include "../../Premake/extensions.lua"

-- include for common stuff 
include "../../Premake/common.lua"

include (dirs.external)
include (dirs.engine)

-------------------------------------------------------------
project "EnhetsTest"
	location (dirs.projectfiles)
	dependson { "External", "Engine" }
		
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}"
	targetdir ("%{dirs.bin}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")
	
	includedirs {"$(VCInstallDir)Auxiliary/VS/UnitTest/include"}
	libdirs { "$(VCInstallDir)Auxiliary/VS/UnitTest/lib"}

	links {"External", "Engine"}

	includedirs { dirs.external, dirs.engine }

	files {
		"source/**.h",
		"source/**.cpp",
	}

	libdirs { dirs.lib, dirs.dependencies }

	verify_or_create_settings("EnhetsTest")
	
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
		kind "SharedLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		
		defines {
			"WIN32",
			"_LIB", 
			"TGE_SYSTEM_WINDOWS" 
		}
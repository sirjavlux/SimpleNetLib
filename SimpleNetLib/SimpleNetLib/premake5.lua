project "SimpleNetLib"

	location "%{dirs.projectfiles}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	toolset "msc-v143"
		
	debugdir "%{dirs.lib}"
	targetdir ("%{dirs.lib}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	files {
		"**.h",
		"**.cpp",
		"**.hpp",
	}

	includedirs {
	
	}

	libdirs {
		"%{dirs.lib}"
	}

	links {
	}

	defines {"_CONSOLE"}
	
    filter "configurations:Debug"
        defines { "_DEBUG", "_ITERATOR_DEBUG_LEVEL=2" }
        runtime "Debug"
        symbols "on"

        flags { "MultiProcessorCompile" }
        buildoptions { "/MDd" } -- Use Multi-threaded Debug DLL

    filter "configurations:Release"
        defines { "_RELEASE", "_ITERATOR_DEBUG_LEVEL=0" }
        runtime "Release"
        optimize "on"

        flags { "MultiProcessorCompile" }
        buildoptions { "/MD" } -- Use Multi-threaded DLL
		
	filter "configurations:Release"
        defines { "_RETAIL" }
        runtime "Release"
        optimize "on"

        flags { "MultiProcessorCompile" }
        buildoptions { "/MD" } -- Use Multi-threaded DLL

	systemversion "latest"
	
	filter "system:windows"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		defines {
			"WIN32",
			"_LIB",
		}
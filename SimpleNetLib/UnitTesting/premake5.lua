project "UnitTesting"
	dependson { "SimpleNetLib" }
	
    files {
        "**.h",
        "**.cpp",
		"../Dependencies/googletest-main/googletest/**.h",
        "../Dependencies/googletest-main/googletest/**.hpp",
        "../Dependencies/googletest-main/googletest/src/gtest-all.cc"
    }
	
	location "%{dirs.projectfiles}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	toolset "msc-v143"
		
	debugdir "%{dirs.bin}"
	targetdir ("%{dirs.bin}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	includedirs {
		"../SimpleNetLib",
		"../Dependencies/googletest-main/googletest/include",
		"../Dependencies/googletest-main/googletest/",
	}

	libdirs {
		"%{dirs.lib}",
	}

	links {
		"SimpleNetLib", 
	}

	defines "_CONSOLE"
	
    filter "configurations:Debug"
        defines "_DEBUG"
        runtime "Debug"
        symbols "on"

        flags { "MultiProcessorCompile" }
        buildoptions { "/MDd" } -- Use Multi-threaded Debug DLL

    filter "configurations:Release"
        defines "_RELEASE"
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
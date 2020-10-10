workspace "sRenderer"
	architecture "x86_64"
	startproject "sRenderer"

	configurations{
		"Debug",
		"Release",
		"Dist"
    }
    
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    project "sRenderer"
		location "sRenderer"
		kind "ConsoleApp"
		language "C++"
		cppdialect "c++17"
		staticruntime "on"


		targetdir("bin/" .. outputdir .. "/%{prj.name}")
        objdir("bin-int/" .. outputdir .. "/%{prj.name}")
        
        files{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/vendor/**.cpp",
			"%{prj.name}/vendor/**.h"
		}
		
		includedirs{
			"%{prj.name}/src",
			"%{prj.name}/vendor",
		}
        
        filter "system:windows"
			staticruntime "on"
			systemversion "latest"


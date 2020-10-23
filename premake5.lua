workspace "sRenderer"
	architecture "x86"
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
		buildoptions "/MDd"


		targetdir("bin/" .. outputdir .. "/%{prj.name}")
        objdir("bin-int/" .. outputdir .. "/%{prj.name}")
        
        files{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/vendor/**.cpp",
			"%{prj.name}/vendor/**.h"
		}
		libdirs{
			"%{prj.name}/vendor/glew/lib/Win32",
			"%{prj.name}/vendor/GLFW/lib",
		}
		links{
			"glfw3.lib", 
			"glew32s.lib",
			"opengl32.lib",
		}

		
		includedirs{
			"%{prj.name}/src",
			"%{prj.name}/vendor",
			"%{prj.name}/vendor/glew/include",
			"%{prj.name}/vendor/imgui",
		}
        
        filter "system:windows"
			staticruntime "on"
			systemversion "latest"


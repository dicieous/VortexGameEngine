workspace "VortexEngine"
	architecture "x64"
	 startproject "Sandbox"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


project "VortexEngine"
	location "VortexEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/"..outputDir.."/%{prj.name}")
	objdir ("bin-int/"..outputDir.."/%{prj.name}")

	pchheader "Vxpch.h"
	pchsource "VortexEngine/src/Vxpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"VX_PLATFORM_WINDOWS",
			"VX_BUILD_DLL"
		}

		postbuildcommands{
			"{RMDIR} ../bin/" .. outputDir .. "/Sandbox",
			"{MKDIR} ../bin/" .. outputDir .. "/Sandbox",
			"{COPYFILE} %{cfg.buildtarget.relpath} ../bin/"..outputDir.."/Sandbox"
		}		

	filter "configurations:Debug"
		defines "VX_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "VX_RELEASE"
		optimize "On"
	
	filter "configurations:Dist"
		defines "VX_DIST"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/"..outputDir.."/%{prj.name}")
	objdir ("bin-int/"..outputDir.."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"VortexEngine/vendor/spdlog/include",
		"VortexEngine/src"
	}

	links{
		"VortexEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"VX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "VX_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "VX_RELEASE"
		optimize "On"
	
	filter "configurations:Dist"
		defines "VX_DIST"
		optimize "On"
workspace "VortexEngine"
	architecture "x64"
	 startproject "Sandbox"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include Directories relative to root folder (solution Directory)
IncludeDir = {}
IncludeDir["Glad"] = "VortexEngine/vendor/Glad/include"
IncludeDir["GLFW"] = "VortexEngine/vendor/GLFW/include"
IncludeDir["ImGui"] = "VortexEngine/vendor/imgui"
IncludeDir["glm"] = "VortexEngine/vendor/glm"
IncludeDir["stb_image"] = "VortexEngine/vendor/stb_image"


group "Dependencies"
	include "VortexEngine/vendor/Glad"
	include "VortexEngine/vendor/GLFW"
	include "VortexEngine/vendor/imgui"

group ""

project "VortexEngine"
	location "VortexEngine"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/"..outputDir.."/%{prj.name}")
	objdir ("bin-int/"..outputDir.."/%{prj.name}")

	pchheader "Vxpch.h"
	pchsource "VortexEngine/src/Vxpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/glm/glm/**.h"
	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"

	}

	links{
		"Glad",
		"GLFW",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"VX_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

		-- postbuildcommands{
		-- 	"{RMDIR} ../bin/" .. outputDir .. "/Sandbox",
		-- 	"{MKDIR} ../bin/" .. outputDir .. "/Sandbox",
		-- 	"{COPYFILE} %{cfg.buildtarget.relpath} ../bin/"..outputDir.."/Sandbox"
		-- }		

	filter "configurations:Debug"
		defines "VX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VX_RELEASE"
		runtime "Release"
		optimize "on"
	
	filter "configurations:Dist"
		defines "VX_DIST"
		runtime "Release"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	staticruntime "on"
	language "C++"

	targetdir ("bin/"..outputDir.."/%{prj.name}")
	objdir ("bin-int/"..outputDir.."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"VortexEngine/vendor/spdlog/include",
		"VortexEngine/src",
		"VortexEngine/vendor",
		"%{IncludeDir.glm}"
	}

	links{
		"VortexEngine",
		"ImGui"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines{
			"VX_PLATFORM_WINDOWS"
		}

	-- Hazel Github issue fix Solution
	-- postbuildcommands
	-- {
	-- 	("{COPYFILE} ../bin/" .. outputDir .. "/VortexEngine/VortexEngine.dll" .. " ../bin/" .. outputDir .. "/%{prj.name}")
	-- }

	filter "configurations:Debug"
		defines "VX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VX_RELEASE"
		runtime "Release"
		optimize "on"
	
	filter "configurations:Dist"
		defines "VX_DIST"
		runtime "Release"
		optimize "on"
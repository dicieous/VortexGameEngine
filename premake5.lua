workspace "VortexEngine"
	architecture "x86_64"
	 startproject "VortexEditor"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

	-- flag
	-- {
	-- 	"MultiProcessorCompile",
	-- }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include Directories relative to root folder (solution Directory)
IncludeDir = {}
IncludeDir["Glad"] = "VortexEngine/vendor/Glad/include"
IncludeDir["GLFW"] = "VortexEngine/vendor/GLFW/include"
IncludeDir["ImGui"] = "VortexEngine/vendor/imgui"
IncludeDir["glm"] = "VortexEngine/vendor/glm"
IncludeDir["stb_image"] = "VortexEngine/vendor/stb_image"
IncludeDir["Entt"] = "VortexEngine/vendor/Entt/include"
IncludeDir["yaml_cpp"] = "VortexEngine/vendor/yaml-cpp/include"


group "Dependencies"
	include "VortexEngine/vendor/Glad"
	include "VortexEngine/vendor/GLFW"
	include "VortexEngine/vendor/imgui"
	include "VortexEngine/vendor/yaml-cpp"

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
		"%{prj.name}/vendor/Entt/include/**.hpp",
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
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.Entt}",
		"%{IncludeDir.yaml_cpp}"

	}

	links{
		"Glad",
		"GLFW",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"VX_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"YAML_CPP_STATIC_DEFINE"
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
	cppdialect "C++17"

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
		"%{IncludeDir.glm}",
		"%{IncludeDir.Entt}"
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


project "VortexEditor"
	location "VortexEditor"
	kind "ConsoleApp"
	staticruntime "on"
	language "C++"
	cppdialect "C++17"

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
		"%{IncludeDir.glm}",
		"%{IncludeDir.Entt}"
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
#pragma once

#include "Vortex/Core/Core.h"
#include "Vortex/Core/Log.h"

#include <string>
#include <filesystem>

namespace Vortex
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		//std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetAssetDirectory()
		{
			VX_CORE_ASSERT(s_ActiveProject, "Project Does not Exist");
			return s_ActiveProject->m_Config.AssetDirectory;
		}

		static ProjectConfig& GetConfig()
		{
			VX_CORE_ASSERT(s_ActiveProject, "Project Does not Exist");
			return s_ActiveProject->m_Config;
		}

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

	private:
		ProjectConfig m_Config;
		inline static Ref<Project> s_ActiveProject;
	};
}
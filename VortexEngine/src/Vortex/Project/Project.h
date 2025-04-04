#pragma once

#include "Vortex/Core/Core.h"
#include "Vortex/Core/Log.h"

#include "Vortex/Assets/RuntimeAssetManager.h"
#include "Vortex/Assets/EditorAssetManager.h"

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

		static const std::filesystem::path& GetProjectDirectory()
		{
			VX_CORE_ASSERT(s_ActiveProject, "Project Does not Exist");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			VX_CORE_ASSERT(s_ActiveProject, "Project Does not Exist");
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			VX_CORE_ASSERT(s_ActiveProject, "Project Does not Exist");
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }

		Ref<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		Ref<RuntimeAssetManager> GetRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManager); }
		Ref<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

	private:
		ProjectConfig m_Config;

		std::filesystem::path m_ProjectDirectory;
		Ref<AssetManagerBase> m_AssetManager;

		inline static Ref<Project> s_ActiveProject;
	};
}
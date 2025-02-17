#include "Vxpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>


namespace Vortex
{
	//Once we have Projects, Change this
	static const std::filesystem::path s_AssetsPath = "Assets";

	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_AssetsPath)
	{
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(s_AssetsPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();

			auto relativePath = std::filesystem::relative(path, s_AssetsPath);
			std::string filePathString = relativePath.filename().string();

			if (directoryEntry.is_directory())
			{
				if (ImGui::Button(filePathString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else
			{
				if (ImGui::Button(filePathString.c_str()))
				{
				}
			}
		}

		ImGui::End();
	}
}
#pragma once

#include <filesystem>

#include "Vortex/Renderer/Texture.h";

namespace Vortex
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		std::filesystem::path m_BaseDirectoryPath;
		std::filesystem::path m_CurrentDirectoryPath;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_BackButton;
	};
}
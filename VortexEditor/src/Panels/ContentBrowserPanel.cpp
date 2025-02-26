#include "Vxpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>


namespace Vortex
{
	static void DrawTextWithEllipsis(const char* text, float maxWidth)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text);
		if (textSize.x <= maxWidth)
		{
			// Center the text if it fits
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (maxWidth - textSize.x) * 0.5f);
			ImGui::TextUnformatted(text);
			return;
		}

		// Text needs truncation
		std::string displayText = text;
		const char* ellipsis = "...";
		float ellipsisWidth = ImGui::CalcTextSize(ellipsis).x;

		while (textSize.x > maxWidth - ellipsisWidth && !displayText.empty())
		{
			displayText.pop_back();
			textSize = ImGui::CalcTextSize(displayText.c_str());
		}

		displayText += ellipsis;
		// Center the truncated text
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (maxWidth - ImGui::CalcTextSize(displayText.c_str()).x) * 0.5f);
		ImGui::TextUnformatted(displayText.c_str());
	}

	//Once we have Projects, Change this
	extern const std::filesystem::path g_AssetsPath = "Assets";

	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(g_AssetsPath)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_DockingSeparatorSize, 0.0f);
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(g_AssetsPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		const float padding = 10.0f;
		static float thumbnailSize = 64;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)panelWidth / cellSize;
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();

			auto relativePath = std::filesystem::relative(path, g_AssetsPath);
			std::string fileNameString = relativePath.filename().string();

			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;

			ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
			ImGui::ImageButton(fileNameString.c_str(), (ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			DrawTextWithEllipsis(fileNameString.c_str(), thumbnailSize);

			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize;
		ImGui::PushStyleVar(ImGuiStyleVar_DockingSeparatorSize, 0.0f);
		ImGui::Begin("##SliderPanel", nullptr, windowFlags);

		// Style the minimal slider
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.11f, 0.11f, 0.11f, 0.6f));         // Barely visible track
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.4f));  // Slightly more visible on hover
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.2f, 0.2f, 0.4f));   // Same as hover
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.8f, 0.8f, 0.8f, 0.8f));      // Light gray handle
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White when active

		// Make the slider thinner
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);    // Square corners    
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 7.0f);      // Smaller grab handle
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f)); // Minimal padding
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 2.0f);     // Slightly rounded handle

		// Position at bottom left
		float windowWidth = ImGui::GetWindowSize().x;
		float sliderWidth = 150.0f; // Adjust width as needed
		//ImGui::SetWindowSize(ImVec2(windowWidth, 10.0f));
		float windowHeight = ImGui::GetWindowSize().y;
		ImGui::SetCursorPos(ImVec2((windowWidth - (sliderWidth + 10.0f)), 6.0f));

		ImGui::PushItemWidth(sliderWidth);
		ImGui::SliderFloat("##ThumbnailSize", &thumbnailSize, 32.0f, 256.0f, "");
		ImGui::PopItemWidth();

		// Pop all style modifications
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(5);

		ImGui::End();	
		ImGui::PopStyleVar(2);

		//TODO:Status Bar
		ImGui::End();
	}
}
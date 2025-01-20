#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Vortex {
	EditorLayer::EditorLayer() :
		Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true) {}

	void EditorLayer::OnAttach()
	{
		m_checkerBoardTexture = Vortex::Texture2D::Create("Assets/Textures/Checkerboard.png");

		Vortex::FrameBufferSpecifications frameSpecs;
		frameSpecs.Width = 1280;
		frameSpecs.Height = 720;

		m_FrameBuffer = Vortex::FrameBuffer::Create(frameSpecs);
		m_ViewportSize = { 1280.0f, 720.0f };
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Vortex::TimeStep timeStep)
	{
		VX_PROFILE_FUNCTION();

		//OnUpdate
		if (m_ViewPortFocused)
		{
			m_CameraController.OnUpdate(timeStep);
		}

		//Rendering
		Vortex::Renderer2D::ResetStats();

		m_FrameBuffer->Bind();
		Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Vortex::RenderCommand::Clear();

		Vortex::Renderer2D::BeginScene(m_CameraController.GetCamera());

		static float rotation = 0.0f;
		rotation += 2.0f;

		Vortex::Renderer2D::DrawRotatedQuads({ -1.0f,1.0f }, { 0.8f,0.8f }, glm::radians(45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Vortex::Renderer2D::DrawQuads({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Vortex::Renderer2D::DrawQuads({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f, 0.2f, 0.3f, 1.0f });
		Vortex::Renderer2D::DrawQuads({ 0.0f, 0.0f, -0.1f }, { 20.0f,20.0f }, m_checkerBoardTexture, 10.0f);
		Vortex::Renderer2D::DrawRotatedQuads({ -2.0f, 0.0f, 0.0f }, { 1.0f,1.0f }, glm::radians(rotation), m_checkerBoardTexture, 20.0f, { 1.0f, 0.9f, 0.9f, 1.0f });

		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f) {

				glm::vec4 color = { ((x + 5.0f) / 10.0f), 0.3f, ((y + 5.0f) / 10.0f),0.7f };
				Vortex::Renderer2D::DrawQuads({ x, y }, { 0.45f,0.45f }, color);
			}
		}

		Vortex::Renderer2D::EndScene();
		m_FrameBuffer->UnBind();
	}

	void EditorLayer::OnImGuiRender()
	{
		VX_PROFILE_FUNCTION();

		auto stats = Vortex::Renderer2D::GetStats();

		static bool dockSpaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Docking"))
			{
				if (ImGui::MenuItem("Exit")) Vortex::Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		ImGui::Begin("Batching Test Rendering");

		ImGui::Text("2D Batch Renderer Stats");

		ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices : %d", stats.GetTotalIndexCount());
		ImGui::Text("Draw Calls : %d", stats.DrawCalls);
		ImGui::Text("Quads : %d", stats.QuadCount);

		ImGui::End();



		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("ViewPort");
		
		ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
		uint32_t texture = m_FrameBuffer->GetAttachementID();

		m_ViewPortFocused = ImGui::IsWindowFocused();
		m_ViewPortHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHovered);
		
		ImGui::Image(texture, ImVec2{ m_ViewportSize.x , m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1, 0 });

		if (m_ViewportSize != *((glm::vec2*)&viewPortPanelSize))
		{
			m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_CameraController.OnResize(viewPortPanelSize.x, viewPortPanelSize.y);
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}


	void EditorLayer::OnEvent(Vortex::Event& event)
	{
		m_CameraController.OnEvent(event);
	}
}

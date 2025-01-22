#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Vortex {
	EditorLayer::EditorLayer() :
		Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true) {}

	void EditorLayer::OnAttach()
	{
		m_checkerBoardTexture = Texture2D::Create("Assets/Textures/Checkerboard.png");

		FrameBufferSpecifications frameSpecs;
		frameSpecs.Width = 1280;
		frameSpecs.Height = 720;

		m_FrameBuffer = FrameBuffer::Create(frameSpecs);
		m_ViewportSize = { 1280.0f, 720.0f };

		m_ActiveScene = CreateRef<Scene>();

		//Entity
		//Vortex::Entity square = m_ActiveScene->CreateEntity();

		//square.AddComponent<TransformComponent>();
		//square.GetComponent<TransfromComponent>();

		m_SquareEntity = m_ActiveScene->CreateEntity();

		m_ActiveScene->Reg().emplace<TransformComponent>(m_SquareEntity);
		m_ActiveScene->Reg().emplace<SpriteRendererComponent>(m_SquareEntity, glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(TimeStep timeStep)
	{
		VX_PROFILE_FUNCTION();

		//OnUpdate
		if (m_ViewPortFocused)
		{
			m_CameraController.OnUpdate(timeStep);
		}


		//Rendering
		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		
		Renderer2D::BeginScene(m_CameraController.GetCamera());

		m_ActiveScene->OnUpdate(timeStep);

		Renderer2D::EndScene();
		m_FrameBuffer->UnBind();
	}

	void EditorLayer::OnImGuiRender()
	{
		VX_PROFILE_FUNCTION();

		auto stats = Renderer2D::GetStats();

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
				if (ImGui::MenuItem("Exit")) Application::Get().Close();

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

		auto& spriteColor = m_ActiveScene->Reg().get<SpriteRendererComponent>(m_SquareEntity).Color;
		ImGui::ColorEdit4("SquareColor", glm::value_ptr(spriteColor));

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("ViewPort");
		
		ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
		uint32_t texture = m_FrameBuffer->GetAttachementID();

		m_ViewPortFocused = ImGui::IsWindowFocused();
		m_ViewPortHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHovered);
		
		ImGui::Image(texture, ImVec2{ m_ViewportSize.x , m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1, 0 });

		if (m_ViewportSize != *((glm::vec2*)&viewPortPanelSize) && viewPortPanelSize.x > 0 && viewPortPanelSize.y > 0)
		{
			m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_CameraController.OnResize(viewPortPanelSize.x, viewPortPanelSize.y);
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}


	void EditorLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
	}
}

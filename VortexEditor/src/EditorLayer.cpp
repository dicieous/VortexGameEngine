#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Vortex {
	EditorLayer::EditorLayer() :
		Layer("EditorLayer"), m_CameraController(1600.0f / 900.0f, true) {}

	void EditorLayer::OnAttach()
	{
		m_checkerBoardTexture = Texture2D::Create("Assets/Textures/Checkerboard.png");

		FrameBufferSpecifications frameSpecs;
		frameSpecs.Width = 1280;
		frameSpecs.Height = 720;

		m_FrameBuffer = FrameBuffer::Create(frameSpecs);
		//m_ViewportSize = { 1280.0f, 720.0f };

		m_ActiveScene = CreateRef<Scene>();


		auto& square = m_ActiveScene->CreateEntity("Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f));

		auto& square2 = m_ActiveScene->CreateEntity("Red Square");
		square2.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCameraEntity = m_ActiveScene->CreateEntity("Second Camera");
		auto& cc = m_SecondCameraEntity.AddComponent<CameraComponent>();
		cc.primary = false;

		class CameraController : public ScriptableEntity {

		public:
			void OnCreate() {
				//std::cout << "CameraController::OnCreate" << std::endl;
			}

			void OnUpdate(TimeStep ts) {
				auto& transform = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;
				if (Input::IsKeyPressed(VX_KEY_A))
					transform.x -= speed * ts;
				if (Input::IsKeyPressed(VX_KEY_D))
					transform.x += speed * ts;
				if (Input::IsKeyPressed(VX_KEY_W))
					transform.y += speed * ts;
				if (Input::IsKeyPressed(VX_KEY_S))
					transform.y -= speed * ts;
			}

			void OnDestroy() {

			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHeirarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		VX_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(TimeStep timeStep)
	{
		VX_PROFILE_FUNCTION();

		FrameBufferSpecifications specs = m_FrameBuffer->GetSpecifications();

		//Resize
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && //zero sized framebuffer is invalid
			(specs.Width != m_ViewportSize.x || specs.Height != m_ViewportSize.y)
			)
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

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

		m_ActiveScene->OnUpdate(timeStep);

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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 300.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		m_SceneHeirarchyPanel.OnImGuiRender();

		ImGui::Begin("Stats");

		ImGui::Text("2DRenderer Stats");

		ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices : %d", stats.GetTotalIndexCount());
		ImGui::Text("Draw Calls : %d", stats.DrawCalls);
		ImGui::Text("Quads : %d", stats.QuadCount);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("ViewPort");


		uint64_t texture = m_FrameBuffer->GetAttachementID();

		m_ViewPortFocused = ImGui::IsWindowFocused();
		m_ViewPortHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHovered);

		ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };

		ImGui::Image(texture, ImVec2{ m_ViewportSize.x , m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1, 0 });

		ImGui::PopStyleVar();
		ImGui::End();
	}


	void EditorLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
	}
}

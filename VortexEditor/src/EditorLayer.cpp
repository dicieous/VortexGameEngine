#include "EditorLayer.h"
#include "Vortex/Scene/SceneSerializer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

#include "Vortex/Utils/PlatformUtils.h"
#include "Vortex/Math/Math.h"

namespace Vortex
{
	extern const std::filesystem::path g_AssetsPath;

	EditorLayer::EditorLayer() :
		Layer("EditorLayer"), m_CameraController(1600.0f / 900.0f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		m_checkerBoardTexture = Texture2D::Create("Assets/Textures/Checkerboard.png");
		m_PlayIconTexture = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_StopIconTexture = Texture2D::Create("Resources/Icons/StopButton.png");

		FrameBufferSpecifications frameSpecs;
		frameSpecs.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		frameSpecs.Width = 1280;
		frameSpecs.Height = 720;

		m_FrameBuffer = FrameBuffer::Create(frameSpecs);

		m_ActiveScene = CreateRef<Scene>();

		m_EditorCamera = EditorCamera(glm::radians(30.0f), 1.778f, 0.1f, 1000.0f);

#if 0
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

		class CameraController : public ScriptableEntity
		{

		public:
			void OnCreate()
			{
				//std::cout << "CameraController::OnCreate" << std::endl;
			}

			void OnUpdate(TimeStep ts)
			{
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

			void OnDestroy()
			{

			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

#endif
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

			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		//OnUpdate

		//Rendering
		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		//Clear EntityID attachment to -1 
		m_FrameBuffer->ClearAttachment(1, -1);

		//Play the Game
		switch (m_SceneState)
		{
		case SceneState::Edit:
			{
				if (m_ViewPortFocused)
				{
					m_CameraController.OnUpdate(timeStep);
				}

				m_EditorCamera.OnUpdate(timeStep);
				m_ActiveScene->OnUpdateEditor(timeStep, m_EditorCamera);

				break;
			}
		case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(timeStep);
				break;
			}
		}

		auto [mouseX, mouseY] = ImGui::GetMousePos();
		mouseX -= m_viewportBounds[0].x;
		mouseY -= m_viewportBounds[0].y;

		glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];

		mouseY = m_ViewportSize.y - mouseY;

		int mouseXPos = (int)mouseX;
		int mouseYPos = (int)mouseY;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y)
		{
			int pixelData = m_FrameBuffer->ReadPixel(1, mouseXPos, mouseYPos);

			if (pixelData != -1 && m_HoveredEntity != Entity((entt::entity)pixelData, m_ActiveScene.get()))
				m_HoveredEntity = Entity((entt::entity)pixelData, m_ActiveScene.get());
			else if (pixelData == -1 && m_HoveredEntity != Entity())
				m_HoveredEntity = Entity();
		}

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
				if (ImGui::MenuItem("New", "CTRL+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "CTRL+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save As...", "CTRL+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit")) Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		m_SceneHeirarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		ImGui::Begin("Stats");

		std::string name = "None";
		TagComponent component;
		if (m_HoveredEntity && m_HoveredEntity.TryGetComponent<TagComponent>(component))
			name = component.Tag;

		ImGui::Text("Hovered Entity: %s", name.c_str());

		ImGui::Text("2DRenderer Stats");

		ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices : %d", stats.GetTotalIndexCount());
		ImGui::Text("Draw Calls : %d", stats.DrawCalls);
		ImGui::Text("Quads : %d", stats.QuadCount);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("ViewPort");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin(); //Include Tab bar offset
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
		m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

		uint64_t texture = m_FrameBuffer->GetColorAttachementRendererID();

		m_ViewPortFocused = ImGui::IsWindowFocused();
		m_ViewPortHovered = ImGui::IsWindowHovered();
		const auto& layer = Application::Get().GetImGuiLayer();

		if (!ImGui::IsAnyItemActive())
			layer->BlockEvents(!m_ViewPortFocused && !m_ViewPortHovered);
		else
			layer->BlockEvents(!m_ViewPortFocused || !m_ViewPortHovered);

		ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };

		ImGui::Image(texture, ImVec2{ m_ViewportSize.x , m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1, 0 });
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payLoad->Data;
				OpenScene(std::filesystem::path(g_AssetsPath) / path);
			}

			ImGui::EndDragDropTarget();
		}

		//Gizmos

		//TODO : Implement Grids for Editor Window

		Entity selectedEntity = m_SceneHeirarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y, m_viewportBounds[1].x - m_viewportBounds[0].x, m_viewportBounds[1].y - m_viewportBounds[0].y);

			//Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			//Transform Data
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			//snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5; // Snap to 0.5m for translation/scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) snapValue = 45.0f; //In Degrees

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType,
				ImGuizmo::MODE::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				tc.Translation = translation;
				tc.Rotation = rotation; //Implement showing rotation in degree above 360 degree while changing it from Imguizmo(Optional, needed when animation keyframe is used)
				tc.Scale = scale;
			}
		}

		ImGui::PopStyleVar();
		
		UI_ToolBar();
		
		ImGui::End();
	}

	void EditorLayer::UI_ToolBar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		
		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_PlayIconTexture : m_StopIconTexture;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		if (ImGui::ImageButton("##icon", (ImTextureID)icon->GetRendererID(), ImVec2(size, size)))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}

		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
		m_EditorCamera.OnEvent(event);

		EventDispacher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(VX_BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(VX_BIND_EVENT_FUNC(EditorLayer::OnMouseButtonPressed));
	}


	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		//Shortcuts
		//TODO: Improve the Shortcuts
		if (event.GetRepeatCount() > 0)
		{
			return false;
		}

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool Shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (event.GetKeyCode())
		{
		case Key::N:
			if (control)
			{
				NewScene();
			}
			break;

		case Key::O:
			if (control)
			{
				OpenScene();
			}
			break;

		case Key::S:
			if (control)
			{
				if (Shift)
					SaveSceneAs();
				else
					SaveScene();
			}
			break;

			//Scene Commands
		case Key::D:
			if (control)
			{
				OnDuplicateEntity();
			}
			break;

			//Gizmos
			if (ImGuizmo::IsUsing())
			{
		case Key::Q:
			m_GizmoType = -1;
			break;

		case Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;

		case Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;

		case Key::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
			}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		//TODO: Make a bool function or something to check for Gizmos selection
		if (event.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewPortHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHeirarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}

		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHeirarchyPanel.SetContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filePath = FileDialogs::OpenFile("Vortex Scene (*.vortex)\0*.vortex\0");

		if (!filePath.empty())
		{
			OpenScene(filePath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& filePath)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (filePath.extension().string() != ".vortex")
		{
			VX_CORE_WARN("Could not Load {0} - Not a Scene File", filePath.filename().string());
			return;
		}
		
		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(filePath.string()))
		{
			m_EditorScene = newScene;
			newScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHeirarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = filePath;
		}

		/*m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHeirarchyPanel.SetContext(m_EditorScene);

		SceneSerializer serialize(m_EditorScene);
		serialize.Deserialize(filePath.string());

		m_ActiveScene = m_EditorScene;*/
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filePath = FileDialogs::SaveFile("Vortex Scene (*.vortex)\0*.vortex\0");

		if (!filePath.empty())
		{
			SerializeScene(m_ActiveScene, filePath);
			m_EditorScenePath = filePath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
		{
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& filePath)
	{
		SceneSerializer serialize(scene);
		serialize.Serialize(filePath.string());
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHeirarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		
		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;

		m_SceneHeirarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;
		
		Entity selectedEntity = m_SceneHeirarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			m_EditorScene->DuplicateEntity(selectedEntity);
		}
	}
	
}

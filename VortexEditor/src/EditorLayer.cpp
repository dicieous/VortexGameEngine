#include "EditorLayer.h"
#include "Vortex/Scene/SceneSerializer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

#include "Vortex/Utils/PlatformUtils.h"
#include "Vortex/Math/Math.h"

namespace Vortex
{

	EditorLayer::EditorLayer() :
		Layer("EditorLayer"), m_CameraController(1600.0f / 900.0f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		m_PlayIconTexture = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_SimulateIconTexture = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_StopIconTexture = Texture2D::Create("Resources/Icons/StopButton.png");

		FrameBufferSpecifications frameSpecs;
		frameSpecs.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		frameSpecs.Width = 1280;
		frameSpecs.Height = 720;

		m_FrameBuffer = FrameBuffer::Create(frameSpecs);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpecifications().commandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			//TODO: Prompt the User to select a Directory
			//NewProject();

			//If no project is opened close VortexEditor
			if (!OpenProject())
				Application::Get().Close();
		}

		m_EditorCamera = EditorCamera(glm::radians(45.0f), 1.778f, 0.1f, 1000.0f);
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
		RenderCommand::SetClearColor({ 0.15f, 0.25f, 0.35f, 1.0f });
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

		case SceneState::Simulate:
		{
			m_EditorCamera.OnUpdate(timeStep);
			m_ActiveScene->OnUpdateSimulation(timeStep, m_EditorCamera);
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
			//VX_CORE_TRACE("Pixeldata: {0}", pixelData);
			if (pixelData != -1 && m_HoveredEntity != Entity((entt::entity)pixelData, m_ActiveScene.get()))
				m_HoveredEntity = Entity((entt::entity)pixelData, m_ActiveScene.get());
			else if (pixelData == -1 && m_HoveredEntity != Entity())
				m_HoveredEntity = Entity();
		}


		Renderer2D::DrawInfiniteGrid(m_InfiniteGridSpecs);

		OnOverlayRender();

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
		style.WindowMinSize.x = 320.0f;
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

				if (ImGui::MenuItem("Open Project", "CTRL+O"))
				{
					OpenProject();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "CTRL+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Save Scene", "CTRL+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save Scene As...", "CTRL+Shift+S"))
				{
					SaveSceneAs();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		m_SceneHeirarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();

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

		glm::vec2 viewportMin = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		glm::vec2 viewportMax = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_viewportBounds[0] = viewportMin;
		m_viewportBounds[1] = viewportMax;


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
				OpenScene(path);
			}

			ImGui::EndDragDropTarget();
		}

		// Floating window constraints
		ImVec2 floatingWindowPos;
		ImVec2 floatingWindowSize;

		ImGui::SetNextWindowBgAlpha(0.8f);


		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize; // Prevents docking if unwanted

		// Begin floating window
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
		ImGui::Begin("##Floating Panel", nullptr, windowFlags);
		ImGui::Dummy({ 1.0f, 5.0f });

		ImVec2 currentPos = ImGui::GetCursorPos();
		float offset = 20.0f;
		ImGui::SetCursorPos({ currentPos.x + offset, currentPos.y });

		ImGui::Text("View Colliders");
		ImGui::SameLine();
		ImGui::Checkbox("##View Colliders", &m_showPhysicsColliders);

		// Get the updated position and size of the window
		floatingWindowPos = ImGui::GetWindowPos();
		floatingWindowSize = ImGui::GetWindowSize();

		// **Clamp the position so it doesn't go outside the viewport**
		floatingWindowPos.x = std::max(viewportMin.x + 5.0f, std::min(floatingWindowPos.x, viewportMax.x - floatingWindowSize.x - 5.0f));
		floatingWindowPos.y = std::max(viewportMin.y + 5.0f, std::min(floatingWindowPos.y, viewportMax.y - floatingWindowSize.y - 5.0f));

		// Apply the clamped position
		ImGui::SetWindowPos(floatingWindowPos);

		ImGui::End();
		ImGui::PopStyleVar();

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

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize);

		float size = ImGui::GetWindowHeight() - 10.0f;
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_PlayIconTexture : m_StopIconTexture;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

			if (ImGui::ImageButton("##Playicon", (ImTextureID)icon->GetRendererID(), ImVec2(size, size)))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}

		ImGui::SameLine();

		{
			/*Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_SimulateIconTexture : m_StopIconTexture;

			if (ImGui::ImageButton("##Simulateicon", (ImTextureID)icon->GetRendererID(), ImVec2(size, size)))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}*/
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
		if (event.IsRepeated())
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
				OpenProject();
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

	void EditorLayer::OnOverlayRender()
	{

		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
				return;

			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		//Colliders Visualizing
		if (m_showPhysicsColliders)
		{
			//Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitesWith<TransformComponent, BoxCollider2DComponent>();

				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.001f))
						* glm::scale(glm::mat4(1.0), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			//Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitesWith<TransformComponent, CircleCollider2DComponent>();

				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.007f, 0.01f);
				}
			}
		}

		Renderer2D::EndScene();
	}


	void EditorLayer::NewProject()
	{
		Project::New();
	}

	bool EditorLayer::OpenProject()
	{
		std::string filePath = FileDialogs::OpenFile("Vortex Project (*.vxproj)\0*.vxproj\0");

		if (filePath.empty()) return false;

		OpenProject(filePath);
		return true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path& filePath)
	{
		if (Project::Load(filePath))
		{
			auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath);
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
		}
	}

	void EditorLayer::SaveProject()
	{
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
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHeirarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = filePath;
		}
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
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
		m_InfiniteGridSpecs.Enabled = false;
		m_SceneHeirarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHeirarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		VX_CORE_ASSERT((m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate), "Invalid Scene");

		switch (m_SceneState)
		{
		case SceneState::Play:
			m_ActiveScene->OnRuntimeStop();
			break;

		case SceneState::Simulate:
			m_ActiveScene->OnSimulationStop();
			break;
		}

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;
		m_InfiniteGridSpecs.Enabled = true;
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

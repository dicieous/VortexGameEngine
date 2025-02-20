#pragma once
#include<Vortex.h>

#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Vortex/Renderer/EditorCamera.h"

namespace Vortex {
	class EditorLayer : public Layer {

	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Vortex::TimeStep timeStep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Vortex::Event& event) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

		void OnOverlayRender();

		void NewScene();
		
		void OpenScene();
		void OpenScene(const std::filesystem::path& filePath);
		
		void SaveSceneAs();
		void SaveScene();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& filePath);

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		//UI ToolBar
		void UI_ToolBar();

	private:
		Vortex::OrthographicCameraController m_CameraController;
		
		//Temp
		Ref<VertexArray> m_squareVA;
		Ref<Texture2D> m_checkerBoardTexture;
		Ref<Shader> m_flatColorShaderSqr;

		bool m_ViewPortFocused = false, m_ViewPortHovered = false;
		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;

		Entity m_CameraEntity;
		Entity m_SecondCameraEntity;
		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;

		bool m_PrimaryCamera = true;

		bool m_showPhysicsColliders = false;

		int m_GizmoType = 7;

		glm::vec2 m_ViewportSize;
		glm::vec3 m_squareColor{ 0.2f, 0.3f, 0.8f };

		glm::vec2 m_viewportBounds[2];

		enum class SceneState
		{
			Edit = 0,
			Play = 1,
		};

		SceneState m_SceneState = SceneState::Edit;

		//Panels
		SceneHeirarchyPanel m_SceneHeirarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		//Editor Resources
		Ref<Texture2D> m_PlayIconTexture;
		Ref<Texture2D> m_StopIconTexture;
	};
}
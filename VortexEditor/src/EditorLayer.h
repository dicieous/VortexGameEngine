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

		void NewScene();
		void OpenScene();
		void SaveSceneAs();

	private:
		Vortex::OrthographicCameraController m_CameraController;

		//Temp
		Ref<VertexArray> m_squareVA;
		Ref<Texture2D> m_checkerBoardTexture;
		Ref<Shader> m_flatColorShaderSqr;

		bool m_ViewPortFocused = false, m_ViewPortHovered = false;
		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;

		Entity m_CameraEntity;
		Entity m_SecondCameraEntity;
		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;

		bool m_PrimaryCamera = true;

		int m_GizmoType = 7;

		glm::vec2 m_ViewportSize;
		glm::vec3 m_squareColor{ 0.2f, 0.3f, 0.8f };

		glm::vec2 m_viewportBounds[2];
		//Panels
		SceneHeirarchyPanel m_SceneHeirarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
	};
}
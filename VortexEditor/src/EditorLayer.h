#pragma once
#include<Vortex.h>

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
		Vortex::OrthographicCameraController m_CameraController;

		//Temp
		Ref<VertexArray> m_squareVA;
		Ref<Texture2D> m_checkerBoardTexture;
		Ref<Shader> m_flatColorShaderSqr;

		bool m_ViewPortFocused = false, m_ViewPortHovered = false;
		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Scene> m_ActiveScene;
		entt::entity  m_SquareEntity;

		glm::vec2 m_ViewportSize;
		glm::vec3 m_squareColor{ 0.2f, 0.3f, 0.8f };
	};
}
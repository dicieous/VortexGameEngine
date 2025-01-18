#pragma once
#include "Vortex/Core/TimeStep.h"
#include "Vortex/Renderer/Camera.h"

#include "Vortex/Events/ApplicationEvent.h"
#include "Vortex/Events/MouseEvent.h"

namespace Vortex {

	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController {

	public:
		OrthographicCameraController(float aspectRatio, bool canRotate = false);

		void OnUpdate(TimeStep& ts);
		void OnEvent(Event& e);

		inline OrthographicCamera& GetCamera() { return m_Camera; }
		inline const OrthographicCamera& GetCamera() const { return m_Camera; }

		inline const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }

		inline float GetZoomLevel() const { return m_ZoomLevel; }
		inline void SetZoomLevel(float level) { m_ZoomLevel = level; CalculateView(); }
	private:
		void CalculateView();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:
		float m_aspectRatio;
		float m_ZoomLevel = 1.0f;
		
		bool m_canRotate;
		
		glm::vec3 m_cameraPositon = { 0.0f, 0.0f,0.0f };
		float m_cameraRotation = 0.0f;

		float m_cameraTranslationSpeed = 1.0f;
		float m_cameraRotationSpeed = 10.0f;

	private:
		OrthographicCamera m_Camera;
		OrthographicCameraBounds m_Bounds;
	};
}

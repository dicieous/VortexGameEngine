#include "Vxpch.h"
#include "OrthographicCameraController.h"

#include "Vortex/Core/Input.h"
#include "Vortex/Core/Keycodes.h"

namespace Vortex {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool canRotate) :
		m_aspectRatio(aspectRatio), m_Camera(-m_aspectRatio * m_ZoomLevel, m_aspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_canRotate(canRotate)
	{

	}

	void OrthographicCameraController::OnUpdate(TimeStep& ts)
	{

		if (Input::IsKeyPressed(VX_KEY_A)) {
			m_cameraPositon.x -= m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(VX_KEY_D)) {
			m_cameraPositon.x += m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(VX_KEY_S)) {
			m_cameraPositon.y += m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(VX_KEY_W)) {
			m_cameraPositon.y -= m_cameraTranslationSpeed * ts;
		}


		if (m_canRotate) {
			if (Input::IsKeyPressed(VX_KEY_Q)) {
				m_cameraRotation += m_cameraRotationSpeed * ts;
			}
			else if (Input::IsKeyPressed(VX_KEY_E)) {
				m_cameraRotation -= m_cameraRotationSpeed * ts;
			}

			m_Camera.SetRotation(m_cameraRotation);
		}


		m_Camera.SetPosition(m_cameraPositon);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispacher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(VX_BIND_EVENT_FUNC(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(VX_BIND_EVENT_FUNC(OrthographicCameraController::OnWindowResized));

	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = glm::max(0.10f, m_ZoomLevel);

		m_Camera.SetProjection(-m_aspectRatio * m_ZoomLevel, m_aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_aspectRatio * m_ZoomLevel, m_aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}
}
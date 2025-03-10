#include "Vxpch.h"
#include "OrthographicCameraController.h"

#include "Vortex/Core/Input.h"
#include "Vortex/Core/Keycodes.h"

namespace Vortex {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool canRotate)
		:m_aspectRatio(aspectRatio), m_Camera(-m_aspectRatio * m_ZoomLevel,
			m_aspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_canRotate(canRotate),
		m_Bounds({ -m_aspectRatio * m_ZoomLevel, m_aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel })
	{

	}

	void OrthographicCameraController::OnUpdate(TimeStep& ts)
	{
		VX_PROFILE_FUNCTION();

		if (Input::IsKeyPressed(Key::A)) {
			m_cameraPositon.x -= m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(Key::D)) {
			m_cameraPositon.x += m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(Key::W)) {
			m_cameraPositon.y += m_cameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(Key::S)) {
			m_cameraPositon.y -= m_cameraTranslationSpeed * ts;
		}


		if (m_canRotate) {
			if (Input::IsKeyPressed(Key::Q)) {
				m_cameraRotation += m_cameraRotationSpeed * ts;
			}
			else if (Input::IsKeyPressed(Key::E)) {
				m_cameraRotation -= m_cameraRotationSpeed * ts;
			}

			m_Camera.SetRotation(m_cameraRotation);
		}


		m_Camera.SetPosition(m_cameraPositon);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		VX_PROFILE_FUNCTION();

		EventDispacher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(VX_BIND_EVENT_FUNC(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(VX_BIND_EVENT_FUNC(OrthographicCameraController::OnWindowResized));

	}

	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_aspectRatio * m_ZoomLevel, m_aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_aspectRatio = width / height;
		CalculateView();
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		VX_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = glm::max(0.10f, m_ZoomLevel);

		CalculateView();

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		VX_PROFILE_FUNCTION();

		OnResize((float)e.GetWidth() , (float)e.GetHeight());
		return false;
	}
}
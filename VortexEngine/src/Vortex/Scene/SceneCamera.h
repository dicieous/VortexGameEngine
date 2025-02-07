#pragma once

#include "Vortex/Renderer/Camera.h"

namespace Vortex {

	class SceneCamera : public Camera
	{

	public:
		enum class ProjectionType {Perspective = 0, Orthographic = 1};
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetPerspectiveVerticalFOV(float verticalFOV) { m_PerspectiveFOV = verticalFOV; RecalculateProjection(); }
		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }

		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }

		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }


		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }

		void SetOrthoGraphicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }

		void SetOrthoGraphicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }


		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.001f, m_PerspectiveFar = 1000.0f;

		float m_AspectRatio = 0.0f;
	};
}
#pragma once

#include <entt.hpp>

#include "Vortex/Core/TimeStep.h"

namespace Vortex {

	class Entity;

	class Scene {

	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdate(TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

	private:
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		entt::registry m_Registry;


		friend class Entity;
		friend class SceneHeirarchyPanel;
	};
}
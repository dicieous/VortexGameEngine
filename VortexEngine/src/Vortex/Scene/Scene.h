#pragma once

#include <entt.hpp>

#include "Vortex/Core/TimeStep.h"

namespace Vortex {

	class Entity;
	class EditorCamera;

	class Scene {

	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateRuntime(TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		entt::registry m_Registry;


		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHeirarchyPanel;
	};
}
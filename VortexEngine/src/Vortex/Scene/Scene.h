#pragma once

#include <entt.hpp>

#include "Vortex/Core/UUID.h"
#include "Vortex/Core/TimeStep.h"


class b2World;

namespace Vortex {

	class Entity;
	class EditorCamera;

	class Scene {

	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();
		
		void OnUpdateRuntime(TimeStep ts);
		void OnUpdateSimulation(TimeStep ts, EditorCamera& camera);
		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity Entity);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void RenderScene(EditorCamera& camera);

	private:
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		entt::registry m_Registry;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHeirarchyPanel;
	};
}
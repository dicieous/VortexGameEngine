#pragma once

#include "Scene.h"

#include <entt.hpp>

namespace Vortex {

	class Entity {

	public:
		Entity() = default;

		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			VX_CORE_ASSERT((!HasComponent<T>()), "Component already Present in the Entity");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)... );
		}

		template<typename T>
		T& GetComponent()
		{
			VX_CORE_ASSERT((HasComponent<T>()), "Component not Present in the Entity")
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool TryGetComponent( T& outComponent)
		{
			if (HasComponent<T>()) {
				outComponent = m_Scene->m_Registry.get<T>(m_EntityHandle);
				return true;
			}

			return false;
		}

		template<typename T>
		bool HasComponent() {

			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			VX_CORE_ASSERT((HasComponent<T>()), "Component not Present in the Entity")
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}
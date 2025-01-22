#pragma once

#include <entt.hpp>

#include "Vortex/Core/TimeStep.h"

namespace Vortex {

	class Scene {

	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		//TEMP
		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(TimeStep ts);

	private:
		entt::registry m_Registry;
	};
}
#pragma once

#include <entt.hpp>

namespace Vortex {

	class Scene {

	public:
		Scene();
		~Scene();

	private:
		entt::registry m_Registry;
	};
}
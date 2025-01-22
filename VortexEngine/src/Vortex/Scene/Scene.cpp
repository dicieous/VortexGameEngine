#include "Vxpch.h"
#include "Scene.h"

#include "Vortex/Scene/Components.h"
#include "Vortex/Renderer/Renderer2D.h"

namespace Vortex {

	Scene::Scene()
	{

	}


	Scene::~Scene()
	{

	}

	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}

	void Scene::OnUpdate(TimeStep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

		for (auto entity : group) {
			auto& [transfrom, spriteRenderer] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawQuads(transfrom, spriteRenderer.Color);
		}
	}
}
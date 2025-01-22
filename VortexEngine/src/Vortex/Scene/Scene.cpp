#include "Vxpch.h"
#include "Scene.h"

#include "Vortex/Scene/Components.h"
#include "Vortex/Renderer/Renderer2D.h"

#include "Entity.h"


namespace Vortex {

	Scene::Scene()
	{

	}


	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		
		auto& tag =  entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
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
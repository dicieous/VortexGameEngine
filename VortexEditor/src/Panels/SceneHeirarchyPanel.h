#pragma once
#include "Vortex/Core/Core.h"
#include "Vortex/Core/Log.h"
#include "Vortex/Scene/Scene.h"
#include "Vortex/Scene/Entity.h"

namespace Vortex {

	class SceneHeirarchyPanel {
	
	public:
		SceneHeirarchyPanel() = default;
		SceneHeirarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		void SetSelectedEntity(Entity entity);
		Entity GetSelectedEntity() const { return m_SelectionContext;  }

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
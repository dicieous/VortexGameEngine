#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Vortex/Core/TimeStep.h"


namespace Vortex {

	class VORTEX_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep timeStep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_debugName; }

	protected:

		std::string m_debugName;

	};
}


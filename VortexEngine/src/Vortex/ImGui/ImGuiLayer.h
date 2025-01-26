#pragma once
#include "Vortex/Core/Layer.h"

#include "Vortex/Events/KeyEvent.h"
#include "Vortex/Events/MouseEvent.h"
#include "Vortex/Events/ApplicationEvent.h"


namespace Vortex {

	class VORTEX_API ImGuiLayer : public Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}
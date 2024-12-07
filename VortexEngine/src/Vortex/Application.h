#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "Events/Event.h"
#include "Window.h"
#include "Vortex/Events/ApplicationEvent.h"

namespace Vortex {

	class VORTEX_API Application
	{
	public:

		Application();

		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:

		std::unique_ptr<Window> m_Window;
		
		bool OnWindowClose(WindowCloseEvent& e);
		bool m_Running = true;

		LayerStack m_LayerStack;
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}


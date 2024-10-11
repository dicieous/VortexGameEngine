#include "Vxpch.h"
#include "Application.h"

#include "Vortex/Events/ApplicationEvent.h"
#include "Vortex/Log.h"


namespace Vortex {
	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application() {

	}

	void Application::Run() {

		while (m_Running) {
			m_Window->OnUpdate();
		}
	}
}


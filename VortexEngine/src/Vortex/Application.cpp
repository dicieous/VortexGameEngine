#include "Vxpch.h"
#include "Application.h"

#include "Vortex/Events/ApplicationEvent.h"
#include "Vortex/Log.h"


namespace Vortex {
	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {

		WindowResizeEvent resize_Event(1280, 720);
		
		VX_TRACE("{}", resize_Event.ToString());
		while (true) {

		}
	}
}


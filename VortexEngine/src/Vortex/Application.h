#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Vortex {
	class VORTEX_API Application
	{
	public:
		Application();

		virtual ~Application();

		void Run();
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}


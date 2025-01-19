#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "Vortex/ImGui/ImGuiLayer.h"

#include "Vortex/Renderer/Camera.h"

#include "Vortex/Events/Event.h"
#include "Window.h"
#include "Vortex/Core/TimeStep.h"

namespace Vortex {

	class VORTEX_API Application
	{
	public:

		Application(const std::string& name = "Vortex App");

		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		void Close();
		
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Scope<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;
		bool m_minimized = false;

		LayerStack m_LayerStack;

		float m_lastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}


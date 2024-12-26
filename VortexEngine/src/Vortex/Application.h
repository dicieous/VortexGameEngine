#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "Vortex/ImGui/ImGuiLayer.h"

#include "Vortex/Renderer/Shader.h"

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

		inline Window& GetWindow() { return *m_Window; }
		
		inline static Application& Get() { return *s_Instance; }
	private:

		std::unique_ptr<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;

		bool OnWindowClose(WindowCloseEvent& e);
		bool m_Running = true;

		unsigned int m_vertexArray, m_vertexBuffer, m_indexBuffer;

		std::unique_ptr<Shader> m_Shader;

		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}


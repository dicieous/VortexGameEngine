#pragma once

#include "Core.h"

#include "LayerStack.h"
#include "Vortex/ImGui/ImGuiLayer.h"

#include "Vortex/Renderer/OrthographicCamera.h"

#include "Vortex/Events/Event.h"
#include "Window.h"
#include "Vortex/Core/TimeStep.h"


int main(int argc, char** argv);


namespace Vortex
{

	struct ApplicationSpecifications
	{ 
		std::string Name = "Vortex Application";
		std::string WorkingDirectory;
	};

	class VORTEX_API Application
	{
	public:

		Application(const ApplicationSpecifications& AppSpecs);

		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		const ApplicationSpecifications& GetSpecifications() const { return m_Specifications; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void Run();

	private:
		ApplicationSpecifications m_Specifications;
		Scope<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;
		bool m_minimized = false;

		LayerStack m_LayerStack;

		float m_lastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}


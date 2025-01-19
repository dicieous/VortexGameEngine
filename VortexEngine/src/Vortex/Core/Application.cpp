#include "Vxpch.h"
#include "Application.h"

#include "Vortex/Renderer/Renderer.h"
#include <GLFW/glfw3.h>


namespace Vortex {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name) {
		VX_PROFILE_FUNCTION();

		VX_CORE_ASSERT(!s_Instance, "Application Already Exist")
			s_Instance = this;

		m_Window = Scope<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallBack(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {
		VX_PROFILE_FUNCTION();

	}

	void Application::PushLayer(Layer* layer) {
		VX_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer) {
		VX_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		VX_PROFILE_FUNCTION();

		EventDispacher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}

		//VX_CORE_TRACE("{0}",e);
	}


	void Application::Run() {
		VX_PROFILE_FUNCTION();

		while (m_Running) {


			VX_PROFILE_SCOPE("RunTime Loop");
			float time = (float)glfwGetTime(); //Should be in something like Platform::GetTime
			TimeStep timeStep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			if (!m_minimized) {

				{
					VX_PROFILE_SCOPE("LayerStack OnUpdates");

					for (Layer* layer : m_LayerStack) {
						layer->OnUpdate(timeStep);
					}
				}

				m_ImGuiLayer->Begin();
				{
					VX_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack) {
						layer->OnImGuiRender();
					}
				}
				m_ImGuiLayer->End();

			}


			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		VX_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_minimized = true;

			return false;
		}

		m_minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}


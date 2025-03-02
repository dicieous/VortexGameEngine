#include "Vxpch.h"
#include "Application.h"
#include "Core.h"

#include "Vortex/Renderer/Renderer.h"
#include <GLFW/glfw3.h>
#include <filesystem>


namespace Vortex {

//#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecifications& AppSpecs)
		:m_Specifications(AppSpecs)
	{
		VX_PROFILE_FUNCTION();

		VX_CORE_ASSERT(!s_Instance, "Application Already Exist")
			s_Instance = this;

		m_Window = Scope<Window>(Window::Create(WindowProps(m_Specifications.Name)));
		m_Window->SetEventCallBack(VX_BIND_EVENT_FUNC(Application::OnEvent));

		if(!m_Specifications.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specifications.WorkingDirectory);

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {
		VX_PROFILE_FUNCTION();
		Renderer::ShutDown();
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
		dispatcher.Dispatch<WindowCloseEvent>(VX_BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(VX_BIND_EVENT_FUNC(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			(*it)->OnEvent(e);
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


#include "Vxpch.h"

#include "WindowsWindow.h"
#include "Vortex/Core/Log.h"

#include "Vortex/Events/ApplicationEvent.h"
#include "Vortex/Events/KeyEvent.h"
#include "Vortex/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

//core#include <glad/glad.h>

namespace Vortex {
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description) {
		VX_CORE_ERROR("GLFW Error {0}: {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props) {
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		VX_PROFILE_FUNCTION();
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		VX_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props) {
		
		VX_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		VX_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);


		if (!s_GLFWInitialized) {
			//TODO: glfwTerminate on system Shutdown
			int success = glfwInit();
			VX_CORE_ASSERT(success, "Could not initialize GLFW! ");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}


		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = CreateScope<OpenGLContext>(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//Set GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Height = height;
				data.Width = width;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keyCode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keyCode);
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int keyCode, int scanCode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(keyCode, scanCode, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(keyCode, scanCode);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(keyCode, scanCode, 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown() {
		VX_PROFILE_FUNCTION();
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate() {

		VX_PROFILE_FUNCTION();
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled) {

		VX_PROFILE_FUNCTION();

		if (enabled) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}

		m_Data.Vsync = enabled;
	}

	bool WindowsWindow::IsVSync() const {
		return m_Data.Vsync;
	}
}
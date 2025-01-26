#include "Vxpch.h"
#include "OpenGLContext.h"
#include "Vortex/Core/Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Vortex {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		VX_CORE_ASSERT(m_WindowHandle, "Window Handle is Null");

	}

	void OpenGLContext::Init()
	{
		VX_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VX_CORE_ASSERT(status, "Failed to Initialize Glad!");

		VX_CORE_INFO("OpenGL Info: ");
		VX_CORE_INFO("OpenGL Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		VX_CORE_INFO("OpenGL Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		VX_CORE_INFO("OpenGL Version: {0}", (const char*)glGetString(GL_VERSION));

#ifdef VX_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
		VX_CORE_ASSERT((versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5)), "Hazel requires at least OpenGL version 4.5!");
#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		VX_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}
}
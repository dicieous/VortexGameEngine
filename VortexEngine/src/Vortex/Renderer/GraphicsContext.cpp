#include "Vxpch.h"
#include "Vortex/Renderer/GraphicsContext.h"

#include "Vortex/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Vortex {

	Scope<GraphicsContext> Vortex::GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently bot supported! ");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}

}

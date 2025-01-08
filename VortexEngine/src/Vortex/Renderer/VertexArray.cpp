#include "Vxpch.h"
#include "VertexArray.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace Vortex {

	Scope<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently bot supported! ");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return CreateScope<OpenGLVertexArray>();
		}
		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
}





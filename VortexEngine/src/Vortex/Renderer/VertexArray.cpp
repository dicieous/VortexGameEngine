#include "Vxpch.h"
#include "VertexArray.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace Vortex {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetCurrentAPI())
		{
		case Vortex::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently bot supported! ");
			return nullptr;

		case Vortex::OpenGL:

			return new OpenGLVertexArray();
		}
		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
}





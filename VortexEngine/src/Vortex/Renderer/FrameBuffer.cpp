#include "Vxpch.h"
#include "FrameBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Vortex {

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecifications& specs)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently not Supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:  

			return CreateRef<OpenGLFrameBuffer>(specs);
		}

		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
}
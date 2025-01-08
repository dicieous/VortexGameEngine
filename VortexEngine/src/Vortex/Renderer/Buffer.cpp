#include "Vxpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Vortex {

	//////////// Vertex Buffer Stuff ////////////////////////////////////////////////////

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently not supported! ");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}


	
	//////////// Index Buffer Stuff ////////////////////////////////////////////////////

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently not supported! ");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return CreateRef<OpenGLIndexBuffer>(indices, size);
		}
		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}

}

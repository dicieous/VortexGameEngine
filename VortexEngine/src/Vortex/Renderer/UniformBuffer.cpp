#include "Vxpch.h"
#include "Vortex/Renderer/RendererAPI.h"

#include "UniformBuffer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Vortex
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			VX_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		VX_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}
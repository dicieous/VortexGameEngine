#include "Vxpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Vortex {

	Ref<Texture2D> Texture2D::Create(const TextureSpecifications& specifications, Buffer data)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently bot supported! ");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return CreateRef<OpenGLTexture2D>(specifications, data);
		}
		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
}

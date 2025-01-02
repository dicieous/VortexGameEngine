#include "Vxpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Vortex {
	Shader* Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently not Supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return new OpenGLShader(filePath);
		}

		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}


	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently not Supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
}
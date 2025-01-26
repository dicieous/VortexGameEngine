#include "Vxpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Vortex {

	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently not Supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return CreateRef<OpenGLShader>(filePath);
		}

		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}


	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetCurrentAPI())
		{
		case RendererAPI::API::None:

			VX_CORE_ASSERT(false, "RenderAPI None is Currently not Supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:

			return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		VX_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}


	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		VX_CORE_ASSERT((!Exists(name)), "Shader is Already Present");
		m_shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filePath)
	{
		auto shader = Shader::Create(filePath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filePath)
	{
		auto shader = Shader::Create(filePath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		VX_CORE_ASSERT(Exists(name), "Shader not found");
		return m_shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_shaders.find(name) != m_shaders.end();
	}
}
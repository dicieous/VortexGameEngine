#pragma once

#include "Vortex/Renderer/Texture.h"

#include <glad/glad.h>

namespace Vortex
{

	class OpenGLTexture2D : public Texture2D
	{

	public:
		OpenGLTexture2D(const TextureSpecifications& specifications, Buffer data);
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecifications& GetTextureSpecifications() const override { return m_Specifications; }
		virtual uint32_t GetWidth() const override { return m_width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual const std::string& GetPath() const override { return m_path; }

		virtual void SetData(Buffer data) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		TextureSpecifications m_Specifications;

		std::string m_path;
		bool m_IsLoaded = false;
		uint32_t m_width, m_Height;
		uint32_t m_RendererID;

		GLenum m_internalFormat, m_dataFormat;
	};
}
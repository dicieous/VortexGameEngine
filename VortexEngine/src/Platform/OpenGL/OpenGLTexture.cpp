#include "Vxpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Vortex {

	namespace Utils
	{
		static GLenum VortexImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case Vortex::ImageFormat::None:
				return GL_NONE;

			case Vortex::ImageFormat::R8:
				return GL_RED;

			case Vortex::ImageFormat::RGB8:
				return GL_RGB;

			case Vortex::ImageFormat::RGBA8:
				return GL_RGBA;

			case Vortex::ImageFormat::RGBA32F:
				return GL_RGBA32F;
			}

			VX_CORE_ASSERT(false, "Format Unknown");

			return 0;
		}

		static GLenum VortexImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case Vortex::ImageFormat::None:
				return GL_NONE;

			case Vortex::ImageFormat::R8:
				return GL_R8;

			case Vortex::ImageFormat::RGB8:
				return GL_RGB8;

			case Vortex::ImageFormat::RGBA8:
				return GL_RGBA8;

			case Vortex::ImageFormat::RGBA32F:
				return GL_RGBA32F;
			}

			VX_CORE_ASSERT(false, "Format Unknown");

			return 0;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecifications& specifications)
		:m_Specifications(specifications), m_width(specifications.Width), m_Height(specifications.Height)
	{
		VX_PROFILE_FUNCTION();

		m_internalFormat = Utils::VortexImageFormatToGLInternalFormat(specifications.Format);
		m_dataFormat = Utils::VortexImageFormatToGLDataFormat(specifications.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_internalFormat, m_width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}


	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) 
		: m_path(path)
	{
		VX_PROFILE_FUNCTION();

		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			VX_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string& path)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		VX_CORE_ASSERT(data, "Failed to Load Image");

		m_width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;

		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_internalFormat = internalFormat;
		m_dataFormat = dataFormat;

		VX_CORE_ASSERT(internalFormat & dataFormat, "Format not Supported");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		VX_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		VX_PROFILE_FUNCTION();

		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		VX_CORE_ASSERT((size == m_width * m_Height * bpp), "Data must be entire texture!");

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_width, m_Height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		VX_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

}

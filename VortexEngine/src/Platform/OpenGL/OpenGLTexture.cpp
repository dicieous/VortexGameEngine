#include "Vxpch.h"
#include "OpenGLTexture.h"

namespace Vortex
{

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

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecifications& specifications, Buffer data)
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

		if (data)
			SetData(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		VX_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(Buffer data)
	{
		VX_PROFILE_FUNCTION();

		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		VX_CORE_ASSERT((data.Size == m_width * m_Height * bpp), "Data must be entire texture!");

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_width, m_Height, m_dataFormat, GL_UNSIGNED_BYTE, data.Data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		VX_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

}

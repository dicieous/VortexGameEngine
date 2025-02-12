#include "Vxpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Vortex
{

	static const uint32_t s_MaxFrameBufferSize = 8192;

	namespace Utils
	{
		static GLenum TextureTarget(bool multiSampled)
		{
			return multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTexture(bool multiSampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multiSampled), count, outID);
		}

		static void BindTexture(bool multiSampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multiSampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multiSampled = samples > 1;
			if (multiSampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multiSampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multiSampled = samples > 1;
			if (multiSampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multiSampled), id, 0);
		}

		static bool IsDepthFormat(FrameBufferTextureFormat format)
		{
			switch (format)
			{
			case Vortex::FrameBufferTextureFormat::DEPTH24STENCIL8:
				return true;
			}

			return false;
		}
	}

	Vortex::OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecifications& specs)
		:m_Specifications(specs)
	{
		for (auto format : m_Specifications.Attachments.AttachmentsList)
		{
			if (!Utils::IsDepthFormat(format.TextureFormat))
				m_ColorAttachmentsSpecs.emplace_back(format);
			else
				m_DepthAttachmentSpec = format;
		}

		Invalidate();
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);

		glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		VX_PROFILE_FUNCTION();

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);

			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_colorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multiSample = m_Specifications.Samples > 1;

		//Attachments

		if (m_ColorAttachmentsSpecs.size())
		{
			m_colorAttachments.resize(m_ColorAttachmentsSpecs.size());
			Utils::CreateTexture(multiSample, m_colorAttachments.data(), m_colorAttachments.size());

			for (size_t i = 0; i < m_colorAttachments.size(); i++)
			{
				Utils::BindTexture(multiSample, m_colorAttachments[i]);
				
				switch (m_ColorAttachmentsSpecs[i].TextureFormat)
				{
				case FrameBufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_colorAttachments[i], m_Specifications.Samples, GL_RGBA8, GL_RGBA, m_Specifications.Width, m_Specifications.Height, i);
					break;
				case FrameBufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(m_colorAttachments[i], m_Specifications.Samples, GL_R32I, GL_RED_INTEGER, m_Specifications.Width, m_Specifications.Height, i);
					break;
				}
			}
		}

		if (m_DepthAttachmentSpec.TextureFormat != FrameBufferTextureFormat::None)
		{
			Utils::CreateTexture(multiSample, &m_DepthAttachment, 1);
			Utils::BindTexture(multiSample, m_DepthAttachment);

			switch (m_DepthAttachmentSpec.TextureFormat)
			{
			case FrameBufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specifications.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specifications.Width, m_Specifications.Height);
				break;
			}
		}

		if (m_colorAttachments.size() > 1)
		{
			VX_CORE_ASSERT((m_colorAttachments.size() <= 4), "Size more than 4");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);
		}
		else if(m_colorAttachments.size() == 0)
		{
			//only Depth-Pass
			glDrawBuffer(GL_NONE);
		}

		VX_CORE_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "FrameBuffer is Incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specifications.Width, m_Specifications.Height);
	}

	void OpenGLFrameBuffer::UnBind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			VX_CORE_WARN("Attempted to resize FrameBuffer to {0}, {1}", width, height);
			return;
		}

		m_Specifications.Width = width;
		m_Specifications.Height = height;

		Invalidate();
	}

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		VX_CORE_ASSERT((attachmentIndex < m_colorAttachments.size()), "Out of Bounds Index");
		int pixelData;

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		return pixelData;
	}

}

#pragma once

#include "Vortex/Renderer/FrameBuffer.h"

namespace Vortex {

	class OpenGLFrameBuffer : public FrameBuffer
	{

	public:
		OpenGLFrameBuffer(const FrameBufferSpecifications& specs);
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		void Invalidate();
		
		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetColorAttachementRendererID(uint32_t index = 0) const override { 
			VX_CORE_ASSERT((index < m_colorAttachments.size()), "Out of index in OpenGLFrameBuffer::m_colorAttachments");  return m_colorAttachments[index]; }

		virtual const FrameBufferSpecifications& GetSpecifications() const override { return m_Specifications; }
	
	private:
		uint32_t m_RendererID = 0;

		FrameBufferSpecifications m_Specifications;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentsSpecs;
		FrameBufferTextureSpecification m_DepthAttachmentSpec;

		std::vector<uint32_t> m_colorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
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

		virtual uint32_t GetAttachementID() const override { return m_ColorAttachment; }

		virtual const FrameBufferSpecifications& GetSpecifications() const override { return m_Specifications; }
	
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;

		FrameBufferSpecifications m_Specifications;
	};
}
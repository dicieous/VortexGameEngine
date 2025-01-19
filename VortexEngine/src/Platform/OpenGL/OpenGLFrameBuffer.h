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

		virtual uint32_t GetAttachementID() const override { return m_ColorAttachment; }

		virtual const FrameBufferSpecifications& GetSpecifications() const override { return m_Specifications; }

		void Invalidate();
	
	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;

		FrameBufferSpecifications m_Specifications;
	};
}
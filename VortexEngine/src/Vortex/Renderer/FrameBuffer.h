#pragma once

#include "Vortex/Core/Core.h"

namespace Vortex {

	enum class FrameBufferTextureFormat
	{
		None,

		//Color
		RGBA8,
		RED_INTEGER,

		//Depth/Stencil
		DEPTH24STENCIL8,

		//Default
		Depth = DEPTH24STENCIL8
	};

	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format)
			:TextureFormat(format) { }

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachmentsList)
			:AttachmentsList(attachmentsList) { }

		std::vector<FrameBufferTextureSpecification> AttachmentsList;
	};

	struct FrameBufferSpecifications
	{
		uint32_t Width = 0, Height = 0;
		FrameBufferAttachmentSpecification Attachments;

		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class FrameBuffer {

	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual uint32_t GetColorAttachementRendererID(uint32_t index = 0) const = 0;

		virtual const FrameBufferSpecifications& GetSpecifications() const = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecifications& specs);

	};
}
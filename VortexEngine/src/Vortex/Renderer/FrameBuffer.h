#pragma once

#include "Vortex/Core/Core.h"

namespace Vortex {

	struct FrameBufferSpecifications
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class FrameBuffer {

	public:
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetAttachementID() const = 0;

		virtual const FrameBufferSpecifications& GetSpecifications() const = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecifications& specs);

	};
}
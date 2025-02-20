#pragma once

#include "RendererAPI.h"

namespace Vortex {

	class RenderCommand {

	public:

		inline static void Init() 
		{
			s_RendereAPI -> Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) 
		{
			s_RendereAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{ 
			return s_RendereAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			return s_RendereAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{ 
			s_RendereAPI->DrawIndexed(vertexArray, indexcount);
		};

		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendereAPI->DrawLines(vertexArray, vertexCount);
		};

	private:
		static Scope<RendererAPI> s_RendereAPI;
	};
}
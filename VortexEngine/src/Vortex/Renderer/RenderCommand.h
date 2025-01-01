#pragma once

#include "RendererAPI.h"

namespace Vortex {

	class RenderCommand {

	public:

		inline static void SetClearColor(const glm::vec4& color)
		{ 
			return s_RendereAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			return s_RendereAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{ 
			s_RendereAPI->DrawIndexed(vertexArray); 
		};

	private:
		static RendererAPI* s_RendereAPI;
	};
}
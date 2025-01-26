#include "Vxpch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"


namespace Vortex {

	Scope<RendererAPI> RenderCommand::s_RendereAPI = RendererAPI::Create();
}
#include "Vxpch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"


namespace Vortex {

	RendererAPI* RenderCommand::s_RendereAPI = new OpenGLRendererAPI;
}
#include "Vxpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Vortex {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:  return GL_FLOAT;
		case ShaderDataType::Float2: return GL_FLOAT;
		case ShaderDataType::Float3: return GL_FLOAT;
		case ShaderDataType::Float4: return GL_FLOAT;
		case ShaderDataType::Mat3:   return GL_FLOAT;
		case ShaderDataType::Mat4:   return GL_FLOAT;
		case ShaderDataType::Int:    return GL_INT;
		case ShaderDataType::Int2:   return GL_INT;
		case ShaderDataType::Int3:   return GL_INT;
		case ShaderDataType::Int4:   return GL_INT;
		case ShaderDataType::Bool:   return GL_BOOL;
		}

		VX_CORE_ASSERT(false, "ShaderType Unknown!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		VX_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() 
	{
		VX_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		VX_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		VX_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		VX_PROFILE_FUNCTION();

		const auto& layoutRef = vertexBuffer->GetLayout();

		VX_CORE_ASSERT(layoutRef.GetElements().size(), "Vertex Buffer has no Layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();


		for (const auto& element : layoutRef) {

			glEnableVertexAttribArray(m_VertexBufferIndex);

			glVertexAttribPointer(m_VertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE, layoutRef.GetStride(), (const void*)(intptr_t)element.Offset);

			m_VertexBufferIndex++;
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		VX_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_indexBuffer = indexBuffer;
	}

}

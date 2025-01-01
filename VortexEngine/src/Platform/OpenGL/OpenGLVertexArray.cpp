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
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() 
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		const auto& layoutRef = vertexBuffer->GetLayout();

		VX_CORE_ASSERT(layoutRef.GetElements().size(), "Vertex Buffer has no Layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();


		uint32_t index = 0;
		for (const auto& element : layoutRef) {

			glEnableVertexAttribArray(index);

			glVertexAttribPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE, layoutRef.GetStride(), (const void*)(intptr_t)element.Offset);

			index++;
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_indexBuffer = indexBuffer;
	}

}

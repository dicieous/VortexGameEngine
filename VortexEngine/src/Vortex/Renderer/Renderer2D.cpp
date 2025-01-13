#include "Vxpch.h"
#include "Renderer2D.h"

#include "VertexArray.h";
#include "Shader.h";
#include "RenderCommand.h"

#include <glm/ext/matrix_transform.hpp>

namespace Vortex {

	struct Renderer2Ddata
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};

	static Renderer2Ddata* s_2Ddata;

	void Renderer2D::Init()
	{
		VX_PROFILE_FUNCTION();

		s_2Ddata = new Renderer2Ddata();

		/////// Square Mesh //////////
		float verticesSqr[5 * 4] = {
			-0.5f,-0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		};

		//m_squareVA.reset(VertexArray::Create());
		s_2Ddata->QuadVertexArray = VertexArray::Create();

		Ref<VertexBuffer> squareVB = VertexBuffer::Create(verticesSqr, sizeof(verticesSqr));

		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"}
			});

		s_2Ddata->QuadVertexArray->AddVertexBuffer(squareVB);


		uint32_t indicesSqr[6] = { 0, 1, 2, 2, 3, 0 };

		Ref<IndexBuffer> squareIB = IndexBuffer::Create(indicesSqr, sizeof(indicesSqr) / sizeof(uint32_t));

		s_2Ddata->QuadVertexArray->SetIndexBuffer(squareIB);

		
		s_2Ddata->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_2Ddata->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_2Ddata->TextureShader = Shader::Create("Assets/Shaders/Texture.shader");
		s_2Ddata->TextureShader->Bind();
		s_2Ddata->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		VX_PROFILE_FUNCTION();

		delete s_2Ddata;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		VX_PROFILE_FUNCTION();

		s_2Ddata->TextureShader->Bind();
		s_2Ddata->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
		VX_PROFILE_FUNCTION();

	}

	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const glm::vec4 color)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const glm::vec4 color)
	{
		VX_PROFILE_FUNCTION();

		s_2Ddata->TextureShader->SetFloat4("u_Color", color);
		s_2Ddata->TextureShader->SetFloat("u_TilingFactor", 1.0f);

		s_2Ddata->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y,1.0f });
		s_2Ddata->TextureShader->SetMat4("u_transform", transform);

		s_2Ddata->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_2Ddata->QuadVertexArray);
	}


	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, texture);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		VX_PROFILE_FUNCTION();

		s_2Ddata->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f)); 
		s_2Ddata->TextureShader->SetFloat("u_TilingFactor", 1.0f);

		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y,1.0f });
		s_2Ddata->TextureShader->SetMat4("u_transform", transform);

		s_2Ddata->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_2Ddata->QuadVertexArray);
	}
}

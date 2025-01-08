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
		Ref<Shader> FlatColorShader;
	};

	static Renderer2Ddata* s_2Ddata;

	void Renderer2D::Init()
	{
		s_2Ddata = new Renderer2Ddata();

		/////// Square Mesh //////////
		float verticesSqr[3 * 4] = {
			-0.5f,-0.5f, 0.0f,
			 0.5f,-0.5f, 0.0f,
			 0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
		};

		//m_squareVA.reset(VertexArray::Create());
		s_2Ddata->QuadVertexArray = VertexArray::Create();

		Ref<VertexBuffer> squareVB = VertexBuffer::Create(verticesSqr, sizeof(verticesSqr));

		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"}
			});

		s_2Ddata->QuadVertexArray->AddVertexBuffer(squareVB);


		uint32_t indicesSqr[6] = { 0, 1, 2, 2, 3, 0 };

		Ref<IndexBuffer> squareIB = IndexBuffer::Create(indicesSqr, sizeof(indicesSqr) / sizeof(uint32_t));

		s_2Ddata->QuadVertexArray->SetIndexBuffer(squareIB);

		s_2Ddata->FlatColorShader = Shader::Create("Assets/Shaders/FlatColorShader.glsl");
	}
	void Renderer2D::Shutdown()
	{
		delete s_2Ddata;
	}

	void Renderer2D::BeginScene(OrthographicCamera& camera)
	{
		s_2Ddata->FlatColorShader->Bind();
		s_2Ddata->FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const glm::vec4 color)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const glm::vec4 color)
	{
		s_2Ddata->FlatColorShader->Bind();
		s_2Ddata->FlatColorShader->SetFloat4("u_Color", color);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y,1.0f });
		s_2Ddata->FlatColorShader->SetMat4("u_transform", transform);

		s_2Ddata->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_2Ddata->QuadVertexArray);
	}
}

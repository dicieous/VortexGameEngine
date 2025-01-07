#include "SandBox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>


SandBox2D::SandBox2D() : 
	Layer("SandBox2D"), m_CameraController(1280.0f / 720.0f, true) {}

void SandBox2D::OnAttach()
{
	/////// Square Mesh //////////
	float verticesSqr[3 * 4] = {
		-0.5f,-0.5f, 0.0f,
		 0.5f,-0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
	};

	m_squareVA.reset(Vortex::VertexArray::Create());

	Vortex::Ref<Vortex::VertexBuffer> squareVB(Vortex::VertexBuffer::Create(verticesSqr, sizeof(verticesSqr)));

	squareVB->SetLayout({
		{Vortex::ShaderDataType::Float3, "a_Position"}
		});

	m_squareVA->AddVertexBuffer(squareVB);


	uint32_t indicesSqr[6] = { 0, 1, 2, 2, 3, 0 };

	Vortex::Ref<Vortex::IndexBuffer> squareIB(Vortex::IndexBuffer::Create(indicesSqr, sizeof(indicesSqr) / sizeof(uint32_t)));
	m_squareVA->SetIndexBuffer(squareIB);

	m_flatColorShaderSqr = Vortex::Shader::Create("Assets/Shaders/FlatColorShader.glsl");
}

void SandBox2D::OnDetach()
{

}

void SandBox2D::OnUpdate(Vortex::TimeStep timeStep)
{
	//OnUpdate

	m_CameraController.OnUpdate(timeStep);

	//Rendering
	Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Vortex::RenderCommand::Clear();

	Vortex::Renderer::BeginScene(m_CameraController.GetCamera());

	m_flatColorShaderSqr->Bind(); 
	std::dynamic_pointer_cast<Vortex::OpenGLShader>(m_flatColorShaderSqr)->UploadUniformFloat3("u_Color", m_squareColor);
	Vortex::Renderer::Submit(m_flatColorShaderSqr, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));


	Vortex::Renderer::EndScene();
}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("FlatColor Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
	ImGui::End();
}

void SandBox2D::OnEvent(Vortex::Event& event)
{
	m_CameraController.OnEvent(event);
}

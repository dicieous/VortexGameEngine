#include <Vortex.h>

#include "imgui/imgui.h"
#include <glm/ext/matrix_transform.hpp>

class ExampleLayer : public Vortex::Layer {

public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, 0.9f, -0.9f), m_cameraPositon(0.0f) {

		m_vertexArray.reset(Vortex::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f,-0.5f,0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f,-0.5f,0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f,0.5f,0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};

		std::shared_ptr<Vortex::VertexBuffer> triangleVB(Vortex::VertexBuffer::Create(vertices, sizeof(vertices)));

		Vortex::BufferLayout layout = {
			{Vortex::ShaderDataType::Float3, "a_Position"},
			{Vortex::ShaderDataType::Float4, "a_Color"},
		};

		triangleVB->SetLayout(layout);

		m_vertexArray->AddVertexBuffer(triangleVB);


		uint32_t indices[3] = { 0, 1, 2 };

		std::shared_ptr<Vortex::IndexBuffer> triangleIB(Vortex::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_vertexArray->SetIndexBuffer(triangleIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 o_Color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				o_Color = vec4(v_Position * 0.5 + 0.5, 1.0);
				o_Color = v_Color;
			}
		)";

		m_Shader = std::shared_ptr<Vortex::Shader>(Vortex::Shader::Create(vertexSrc, fragmentSrc));


		/////// Square Mesh //////////
		float verticesSqr[3 * 4] = {
			-0.5f,-0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
		};

		m_squareVA.reset(Vortex::VertexArray::Create());

		std::shared_ptr<Vortex::VertexBuffer> squareVB(Vortex::VertexBuffer::Create(verticesSqr, sizeof(verticesSqr)));

		squareVB->SetLayout({
			{
				Vortex::ShaderDataType::Float3, "a_Position"},
			});

		m_squareVA->AddVertexBuffer(squareVB);


		uint32_t indicesSqr[6] = { 0, 1, 2, 2, 3, 0 };

		std::shared_ptr<Vortex::IndexBuffer> squareIB(Vortex::IndexBuffer::Create(indicesSqr, sizeof(indicesSqr) / sizeof(uint32_t)));
		m_squareVA->SetIndexBuffer(squareIB);


		std::string blueVertexSrcSqr = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_transform * vec4(a_Position, 1.0);
			}
		)";

		std::string blueFragmentSrcSqr = R"(
			#version 330 core

			layout(location = 0) out vec4 o_Color;

			in vec3 v_Position;

			void main()
			{
				o_Color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_ShaderSqr = std::shared_ptr<Vortex::Shader>(Vortex::Shader::Create(blueVertexSrcSqr, blueFragmentSrcSqr));

	}

	void OnUpdate(Vortex::TimeStep timeStep) override
	{

		if (Vortex::Input::IsKeyPressed(VX_KEY_LEFT)) {
			m_cameraPositon.x -= m_cameraSpeed * timeStep;
		}
		else if (Vortex::Input::IsKeyPressed(VX_KEY_RIGHT)) {
			m_cameraPositon.x += m_cameraSpeed * timeStep;
		}
		else if (Vortex::Input::IsKeyPressed(VX_KEY_UP)) {
			m_cameraPositon.y += m_cameraSpeed * timeStep;
		}
		else if (Vortex::Input::IsKeyPressed(VX_KEY_DOWN)) {
			m_cameraPositon.y -= m_cameraSpeed * timeStep;
		}

		if (Vortex::Input::IsKeyPressed(VX_KEY_A)) {
			m_Rotation += m_RotationSpeed * timeStep;
		}
		else if (Vortex::Input::IsKeyPressed(VX_KEY_D)) {
			m_Rotation -= m_RotationSpeed * timeStep;
		}

		Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Vortex::RenderCommand::Clear();

		m_Camera.SetPosition(m_cameraPositon);
		m_Camera.SetRotation(m_Rotation);

		Vortex::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (int i = 0; i < 20; i++) {

			for (int j = 0; j < 20; j++) 
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Vortex::Renderer::Submit(m_ShaderSqr, m_squareVA, transform);
			}
		}

		Vortex::Renderer::Submit(m_Shader, m_vertexArray);

		Vortex::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override {

	}

	void OnEvent(Vortex::Event& event) override
	{
	}

private:

	std::shared_ptr<Vortex::Shader> m_Shader;
	std::shared_ptr<Vortex::VertexArray> m_vertexArray;

	std::shared_ptr<Vortex::VertexArray> m_squareVA;
	std::shared_ptr<Vortex::Shader> m_ShaderSqr;

	Vortex::OrthographicCamera m_Camera;

	glm::vec3 m_cameraPositon;
	float m_cameraSpeed = 1.0f;

	float m_Rotation = 0.0f;
	float m_RotationSpeed = 10.0f;
};

class SandBox : public Vortex::Application {
public:
	SandBox() {
		PushLayer(new ExampleLayer());
	}

	~SandBox() {

	}
};

Vortex::Application* Vortex::CreateApplication() {

	return new SandBox();
}
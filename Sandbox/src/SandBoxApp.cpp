#include <Vortex.h>

#include "imgui/imgui.h"

class ExampleLayer : public Vortex::Layer {

public:
	ExampleLayer() : Layer("Example") {

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

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
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

		m_Shader = std::make_unique<Vortex::OpenGLShader>(vertexSrc, fragmentSrc);



		/////// Square Mesh //////////
		float verticesSqr[3 * 4] = {
			-0.75f,-0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f, 0.75f, 0.0f,
			-0.75f, 0.75f, 0.0f,
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

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
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

		m_ShaderSqr = std::make_unique<Vortex::OpenGLShader>(blueVertexSrcSqr, blueFragmentSrcSqr);
	}

	void OnUpdate() override
	{
		Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Vortex::RenderCommand::Clear();

		Vortex::Renderer::BeginScene();

		m_ShaderSqr->Bind();
		Vortex::Renderer::Submit(m_squareVA);

		m_Shader->Bind();
		Vortex::Renderer::Submit(m_vertexArray);

		Vortex::Renderer::EndScene();

		//VX_INFO("ExampleLayer::Update");
		if (Vortex::Input::IsKeyPressed(VX_KEY_SPACE)) {
			VX_CORE_TRACE("Space Key Pressed");
		}
	}

	virtual void OnImGuiRender() override {
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Vortex::Event& event) override
	{
		//VX_TRACE("{0}", event);
	}

private:

	std::shared_ptr<Vortex::OpenGLShader> m_Shader;
	std::shared_ptr<Vortex::VertexArray> m_vertexArray;

	std::shared_ptr<Vortex::VertexArray> m_squareVA;
	std::shared_ptr<Vortex::OpenGLShader> m_ShaderSqr;
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
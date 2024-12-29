#include "Vxpch.h"
#include "Application.h"

#include "Vortex/Log.h"
#include "Vortex/Renderer/Renderer.h"

namespace Vortex {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {

		VX_CORE_ASSERT(!s_Instance, "Application Already Exist")
			s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);

		m_vertexArray.reset(VertexArray::Create());	

		float vertices[3 * 7] = {
			-0.5f,-0.5f,0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f,-0.5f,0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f,0.5f,0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};

		std::shared_ptr<VertexBuffer> triangleVB(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}, 
		};

		triangleVB->SetLayout(layout);

		m_vertexArray->AddVertexBuffer(triangleVB);


		uint32_t indices[3] = { 0, 1, 2 };

		std::shared_ptr<IndexBuffer> triangleIB(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
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

		m_Shader = std::make_unique<Shader>(vertexSrc, fragmentSrc);



		/////// Square Mesh //////////
		float verticesSqr[3 * 4] = {
			-0.75f,-0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f, 0.75f, 0.0f,
			-0.75f, 0.75f, 0.0f,
		};

		m_squareVA.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> squareVB(VertexBuffer::Create(verticesSqr, sizeof(verticesSqr)));

		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			});

		m_squareVA->AddVertexBuffer(squareVB);


		uint32_t indicesSqr[6] = { 0, 1, 2, 2, 3, 0};

		std::shared_ptr<IndexBuffer> squareIB(IndexBuffer::Create(indicesSqr, sizeof(indicesSqr) / sizeof(uint32_t)));
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

		m_ShaderSqr= std::make_unique<Shader>(blueVertexSrcSqr, blueFragmentSrcSqr);
	}

	Application::~Application() {

	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer) {
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispacher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}

		//VX_CORE_TRACE("{0}",e);
	}


	void Application::Run() {

		while (m_Running) {

			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();

			Renderer::BeginScene();

			m_ShaderSqr->Bind();
			Renderer::Submit(m_squareVA);

			m_Shader->Bind();
			Renderer::Submit(m_vertexArray);

			Renderer::EndScene();
			
			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
}


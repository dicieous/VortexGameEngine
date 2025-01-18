#include "SandBox2D.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

SandBox2D::SandBox2D() :
	Layer("SandBox2D"), m_CameraController(1280.0f / 720.0f, true) {}

void SandBox2D::OnAttach()
{
	m_checkerBoardTexture = Vortex::Texture2D::Create("Assets/Textures/Checkerboard.png");
}

void SandBox2D::OnDetach()
{

}

void SandBox2D::OnUpdate(Vortex::TimeStep timeStep)
{
	VX_PROFILE_FUNCTION();
	//OnUpdate

	m_CameraController.OnUpdate(timeStep);

	//Rendering
	Vortex::Renderer2D::ResetStats();

	Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Vortex::RenderCommand::Clear();

	Vortex::Renderer2D::BeginScene(m_CameraController.GetCamera());

	static float rotation = 0.0f;
	rotation += 2.0f;

	Vortex::Renderer2D::DrawRotatedQuads({ -1.0f,1.0f }, { 0.8f,0.8f }, glm::radians(45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
	Vortex::Renderer2D::DrawQuads({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Vortex::Renderer2D::DrawQuads({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f, 0.2f, 0.3f, 1.0f });
	Vortex::Renderer2D::DrawQuads({ 0.0f, 0.0f, -0.1f }, { 20.0f,20.0f }, m_checkerBoardTexture, 10.0f);
	Vortex::Renderer2D::DrawRotatedQuads({ -2.0f, 0.0f, 0.0f }, { 1.0f,1.0f }, glm::radians(rotation), m_checkerBoardTexture, 20.0f, { 1.0f, 0.9f, 0.9f, 1.0f });

	for (float y = -5.0f; y < 5.0f; y += 0.5f) {
		for (float x = -5.0f; x < 5.0f; x += 0.5f) {

			glm::vec4 color = { ((x + 5.0f) / 10.0f), 0.3f, ((y + 5.0f) / 10.0f),0.7f };
			Vortex::Renderer2D::DrawQuads({ x, y }, { 0.45f,0.45f }, color);
		}
	}

	Vortex::Renderer2D::EndScene();
}

void SandBox2D::OnImGuiRender()
{
	VX_PROFILE_FUNCTION();

	auto stats = Vortex::Renderer2D::GetStats();


	static bool dockSpaceOpen = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Docking"))
		{
			if (ImGui::MenuItem("Exit")) Vortex::Application::Get().Close();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("Batching Test Rendering");

	ImGui::Text("2D Batch Renderer Stats");

	ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices : %d", stats.GetTotalIndexCount());
	ImGui::Text("Draw Calls : %d", stats.DrawCalls);
	ImGui::Text("Quads : %d", stats.QuadCount);

	ImGui::Image(m_checkerBoardTexture->GetRendererID(), ImVec2{ 256.0f, 256.0f });

	ImGui::End();

	ImGui::End();
}

void SandBox2D::OnEvent(Vortex::Event& event)
{
	m_CameraController.OnEvent(event);
}

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
	ImGui::Begin("Batching Test Rendering");

	ImGui::Text("2D Batch Renderer Stats");

	ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices : %d", stats.GetTotalIndexCount());
	ImGui::Text("Draw Calls : %d", stats.DrawCalls);
	ImGui::Text("Quads : %d", stats.QuadCount);

	ImGui::End();

}

void SandBox2D::OnEvent(Vortex::Event& event)
{
	m_CameraController.OnEvent(event);
}

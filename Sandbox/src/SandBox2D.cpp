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

	{
		VX_PROFILE_SCOPE("CameraController");
		m_CameraController.OnUpdate(timeStep);
	}
	//Rendering

	{
		VX_PROFILE_SCOPE("RenderPrep");
		Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Vortex::RenderCommand::Clear();
	}

	{
		VX_PROFILE_SCOPE("SandBox2D::BeginScene");
		Vortex::Renderer2D::BeginScene(m_CameraController.GetCamera());
	}

	{
		VX_PROFILE_SCOPE("RenderDraw");
		Vortex::Renderer2D::DrawQuads({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Vortex::Renderer2D::DrawQuads({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f, 0.2f, 0.3f, 1.0f });
		Vortex::Renderer2D::DrawQuads({ 0.0f,0.0f, -0.1f }, { 5.0f,5.0f }, m_checkerBoardTexture);
	}


	Vortex::Renderer2D::EndScene();

}

void SandBox2D::OnImGuiRender()
{
	VX_PROFILE_FUNCTION();

	ImGui::Begin("FlatColor Settings");
	//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));

	ImGui::End();
}

void SandBox2D::OnEvent(Vortex::Event& event)
{
	m_CameraController.OnEvent(event);
}

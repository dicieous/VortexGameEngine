#include "SandBox2D.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

class Timer {

public:
	Timer(const char* name)
		: m_Name(name), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;
		std::cout << "Duration: " << (end - start) << "ms" << std::endl;
	}

private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

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
	//OnUpdate

	m_CameraController.OnUpdate(timeStep);

	//Rendering
	Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Vortex::RenderCommand::Clear();

	Vortex::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Vortex::Renderer2D::DrawQuads({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Vortex::Renderer2D::DrawQuads({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f, 0.2f, 0.3f, 1.0f });
	Vortex::Renderer2D::DrawQuads({ 0.0f,0.0f, -0.1f }, { 5.0f,5.0f }, m_checkerBoardTexture);


	Vortex::Renderer2D::EndScene();

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

#include "SandBox2D.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

template<typename Fn>
class Timer {

public:
	Timer(const char* name, Fn&& func)
		: m_Name(name), m_Func(func), m_Stopped(false)
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

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	}

private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;

	Fn m_Func;

};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResults profileResult) { m_profileResults.push_back(profileResult); })

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
	PROFILE_SCOPE("OnUpdate");
	//OnUpdate

	{
		PROFILE_SCOPE("CameraController");
		m_CameraController.OnUpdate(timeStep);
	}
	//Rendering

	{
		PROFILE_SCOPE("RenderPrep");
		Vortex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Vortex::RenderCommand::Clear();
	}
	{
		PROFILE_SCOPE("BeginScene");
		Vortex::Renderer2D::BeginScene(m_CameraController.GetCamera());
	}

	{
		PROFILE_SCOPE("RenderDraw");
		Vortex::Renderer2D::DrawQuads({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Vortex::Renderer2D::DrawQuads({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f, 0.2f, 0.3f, 1.0f });
		Vortex::Renderer2D::DrawQuads({ 0.0f,0.0f, -0.1f }, { 5.0f,5.0f }, m_checkerBoardTexture);
	}


	Vortex::Renderer2D::EndScene();

}

void SandBox2D::OnImGuiRender()
{
	ImGui::Begin("FlatColor Settings");
	//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));

	for (auto& result : m_profileResults) {

		char label[50];
		strcpy(label, "%.3fms  ");
		strcat(label, result.Name);

		ImGui::Text(label, result.Time);
	}

	m_profileResults.clear();

	ImGui::End();
}

void SandBox2D::OnEvent(Vortex::Event& event)
{
	m_CameraController.OnEvent(event);
}

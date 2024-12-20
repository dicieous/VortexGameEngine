#include <Vortex.h>

#include "imgui/imgui.h"

class ExampleLayer : public Vortex::Layer {

public:
	ExampleLayer() : Layer("Example") {
		
	}

	void OnUpdate() override
	{
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
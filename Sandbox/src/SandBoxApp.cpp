#include <Vortex.h>


class ExampleLayer : public Vortex::Layer {

public:
	ExampleLayer() : Layer("Example") {

	}

	void OnUpdate() override
	{
		VX_INFO("ExampleLayer::Update");
	}

	void OnEvent(Vortex::Event& event) override
	{
		VX_TRACE("{0}", event);
	}
};

class SandBox : public Vortex::Application {
public:
	SandBox() {
		PushLayer(new ExampleLayer());
		PushOverlay(new Vortex::ImGuiLayer());
	}

	~SandBox() {

	}
};

Vortex::Application* Vortex::CreateApplication() {

	return new SandBox();
}
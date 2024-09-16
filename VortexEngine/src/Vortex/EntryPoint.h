#pragma once

#ifdef VX_PLATFORM_WINDOWS

extern Vortex::Application* Vortex::CreateApplication();

int main(int argc, char** argv) 
{
	Vortex::Log::Init();
	VX_CORE_WARN("Initialized Log");

	int a = 5;
	VX_CORE_INFO("Hello! Var = {0}" ,a);

	auto app = Vortex::CreateApplication();
	app->Run();

	delete app;
}
#endif // VORTEX_PLATFORM_WINDOWS

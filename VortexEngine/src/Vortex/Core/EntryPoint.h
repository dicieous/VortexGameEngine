#pragma once

#ifdef VX_PLATFORM_WINDOWS

extern Vortex::Application* Vortex::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) 
{
	Vortex::Log::Init();
	//VX_CORE_WARN("Initialized Log");

	//int a = 5;
	//VX_CORE_INFO("Hello! Var = {0}" ,a);
	VX_PROFILE_BEGIN_SESSION("StartUp", "Vortex-Startup.json");
	auto app = Vortex::CreateApplication({argc, argv});
	VX_PROFILE_END_SESSION();
	
	VX_PROFILE_BEGIN_SESSION("RunTime", "Vortex-RunTime.json");
	app->Run();
	VX_PROFILE_END_SESSION();

	VX_PROFILE_BEGIN_SESSION("Shutdown", "Vortex-ShutDown.json");
	delete app;
	VX_PROFILE_END_SESSION();
}
#endif // VORTEX_PLATFORM_WINDOWS

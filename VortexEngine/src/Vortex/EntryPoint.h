#pragma once

#ifdef VX_PLATFORM_WINDOWS

extern Vortex::Application* Vortex::CreateApplication();

int main(int argc, char** argv) 
{
	auto app = Vortex::CreateApplication();
	app->Run();
	delete app;
}
#endif // VORTEX_PLATFORM_WINDOWS

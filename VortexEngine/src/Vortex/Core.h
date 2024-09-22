#pragma once

#ifdef VX_PLATFORM_WINDOWS
#ifdef VX_BUILD_DLL
#define VORTEX_API __declspec(dllexport)
#else 
#define VORTEX_API __declspec(dllimport)
#endif
#else
#error Vortex only Support Windows
#endif // VORTEX_PLATFORM_WINDOWS

#define BIT(x) (1 << x)
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

#ifdef VX_ENABLE_ASSERTS
	#define VX_ASSERT(x, ...) {if(!x){VX_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define VX_CORE_ASSERT(x, ...){if(!x){VX_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define VX_ASSERT(x, ...)
	#define VX_CORE_ASSERT(x, ...)
#endif // VX_ENABLE_ASSERTS

#define VX_BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

#define BIT(x) (1 << x)
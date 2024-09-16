#pragma once

#include<memory>

#include "Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

//Core Log Macros
#define VX_CORE_TRACE(...)	::Vortex::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define VX_CORE_INFO(...)	::Vortex::Log::GetCoreLogger()->info(__VA_ARGS__);
#define VX_CORE_WARN(...)	::Vortex::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define VX_CORE_ERROR(...)	::Vortex::Log::GetCoreLogger()->error(__VA_ARGS__);
#define VX_CORE_FATAL(...)	::Vortex::Log::GetCoreLogger()->fatal(__VA_ARGS__);

//Client Log Macros
#define VX_CLIENT_TRACE(...)	::Vortex::Log::GetClientLogger()->trace(__VA_ARGS__);
#define VX_CLIENT_INFO(...)		::Vortex::Log::GetClientLogger()->info(__VA_ARGS__);
#define VX_CLIENT_WARN(...)		::Vortex::Log::GetClientLogger()->warn(__VA_ARGS__);
#define VX_CLIENT_ERROR(...)	::Vortex::Log::GetClientLogger()->error(__VA_ARGS__);
#define VX_CLIENT_FATAL(...)	::Vortex::Log::GetClientLogger()->fatal(__VA_ARGS__);

namespace Vortex {
	class VORTEX_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
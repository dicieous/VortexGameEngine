#pragma once
#include "Vxpch.h"

#include "Vortex/Core/Core.h"
#include "Vortex/Events/Event.h"

namespace Vortex {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Vortex Engine",
			uint32_t width = 1280,
			uint32_t height = 720)
			:Title(title), Width(width), Height(height) {}
	};

	//Interface representing a desktop system based window
	class VORTEX_API Window {

	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {};

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallBack(const EventCallbackFn& eventCallBack) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());

	};
}

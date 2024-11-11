#pragma once

#include "Vortex/Window.h"

#include "GLFW/glfw3.h"

namespace Vortex {

	class WindowsWindow : public Window
	{

	public:
		WindowsWindow(const WindowProps& props);

		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		//Windows Attributes
		void SetEventCallBack(const EventCallbackFn& eventCallBack) override { m_Data.EventCallback = eventCallBack; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool Vsync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
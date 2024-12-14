#pragma once
#include "Vortex/Layer.h"

#include "Vortex/Events/KeyEvent.h"
#include "Vortex/Events/MouseEvent.h"
#include "Vortex/Events/ApplicationEvent.h"

#include "imgui.h"


namespace Vortex {

	class VORTEX_API ImGuiLayer : public Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

	private:

		static ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);
		void ImGui_ImplGlfw_UpdateKeyModifiers(int KeyCode);

		bool OnMousePressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleaseEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrollEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

	private:
		float m_Time = 0.0f;
	};
}
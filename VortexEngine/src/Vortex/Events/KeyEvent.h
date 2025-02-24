#pragma once

#include "Event.h"


namespace Vortex {

	class VORTEX_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
		inline int GetScanCode() const { return m_scanCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);

	protected:

		KeyEvent(int keyCode, int scanCode = 0) : m_KeyCode(keyCode), m_scanCode(scanCode) {}

		int m_KeyCode;
		int m_scanCode;
	};

	class VORTEX_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keyCode, int scanCode, bool IsRepeated) 
			: KeyEvent(keyCode, scanCode), m_IsRepeated(IsRepeated) {}

		inline bool IsRepeated() const { return m_IsRepeated; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_IsRepeated << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeated;
	};

	class VORTEX_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keyCode) : KeyEvent(keyCode){}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};

	class VORTEX_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keyCode, int scanCode) : KeyEvent(keyCode, scanCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}
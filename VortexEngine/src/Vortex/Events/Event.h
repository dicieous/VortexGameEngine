#pragma once

#include "Vxpch.h"
#include "Vortex/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Vortex {
	//Events in Hazel are currently blocking, meaning when an event occurs it
	//immediately gets dispatched and must be dealt with right then an there.
	//For the future, a better strategy might be to buffer events in an event
	//bus and process them during the "event" part of the update stage.

	enum class EventType
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		Apptick,
		AppUpdate,
		AppRender,
		KeyPressed,
		KeyTyped,
		KeyReleased,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication =	BIT(0),
		EventCategoryInput =		BIT(1),
		EventCategoryKeyboard =		BIT(2),
		EventCategoryMouse =		BIT(3),
		EventCategoryMouseButton =	BIT(4),
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() {return EventType :: type;}\
								virtual EventType GetEventType() const override {return GetStaticType();}\
								virtual const char* GetEventName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category; }

	class VORTEX_API Event
	{
		friend class EventDispacher;
	public:
		bool Handled = false;
		
		virtual EventType GetEventType() const = 0;
		virtual const char* GetEventName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetEventName(); };

		inline bool IsInCategory(EventCategory category) const {
			return GetCategoryFlags()& category;
		}

	protected:
	};

	class EventDispacher {

		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispacher(Event& event) : m_Event(event) { }

		template<typename T>
		bool Dispatch(EventFn<T> func) {
			if (m_Event.GetEventType() == T::GetStaticType()) {
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}

			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator <<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}

	template <typename T>
	struct fmt::formatter<T, std::enable_if_t<std::is_base_of<Vortex::Event, T>::value, char>> : fmt::formatter<std::string> {
		auto format(const Vortex::Event& event, format_context& ctx) const {
			return fmt::formatter<std::string>::format(event.ToString(), ctx);
		}
	};
}
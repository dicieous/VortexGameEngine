#pragma once


namespace Vortex {

	class TimeStep {

	public:
		TimeStep(float timeStep = 0.0f) : m_time(timeStep) {}

		float GetSeconds() const { return m_time; }
		float GetMilliSeconds() const { return m_time * 1000.0f; };

		operator float() { return m_time; }

		TimeStep operator +(const TimeStep& time) {
			return TimeStep(m_time + time.m_time);
		}

		TimeStep operator *(const TimeStep& time) {
			return TimeStep(m_time * time.m_time);
		}

	private:
		float m_time;
	};
}
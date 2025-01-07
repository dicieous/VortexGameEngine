#pragma once

#include "Vortex/Core/Input.h"

namespace Vortex {

	class WindowsInput : public Input
	{
	public:
	protected:

		virtual bool IsKeyPressedImpl(int keyCode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;

		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	private:

	};

	
}
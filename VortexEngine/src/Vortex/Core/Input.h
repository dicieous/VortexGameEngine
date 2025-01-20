#pragma once

#include "Core.h"
#include "Keycodes.h"
#include "MouseCodes.h"

namespace Vortex {

	class VORTEX_API Input
	{
	
	public:
		static bool IsKeyPressed(int keyCode);
		static bool IsMouseButtonPressed(int button);

		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
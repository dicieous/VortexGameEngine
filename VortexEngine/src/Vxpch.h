#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Vortex/Core/Log.h"

#include "Vortex/Debug/Instrumentor.h"

#ifdef VX_PLATFORM_WINDOWS

#define NOMINMAX
#include <Windows.h>
#endif // VX_PLATFORM_WINDOWS

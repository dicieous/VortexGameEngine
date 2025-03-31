#pragma once

#include "Vortex/Assets/Asset.h"
#include <filesystem>

namespace Vortex
{
	struct AssetMetaData
	{
		AssetType Type;
		std::filesystem::path FilePath;

		operator bool() const { return Type != AssetType::None; }
	};
}
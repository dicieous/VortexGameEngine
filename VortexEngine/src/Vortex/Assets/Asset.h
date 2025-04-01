#pragma once

#include "Vortex/Core/UUID.h"
#include "Vortex/Core/Core.h"

namespace Vortex
{
	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None,
		Scene,
		Texture2D,
	};

	class Asset
	{
	public:
		AssetHandle Handle; //Generate Handle

		virtual AssetType GetType() const = 0;
	private:

	};
}
#pragma once

#include <Vortex/Core/UUID.h>

namespace Vortex
{
	using AssetHandle = UUID;

	enum class AssetType
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
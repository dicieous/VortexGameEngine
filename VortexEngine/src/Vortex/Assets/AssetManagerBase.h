#pragma once

#include "Asset.h"
#include "Vortex/Core/Core.h"

namespace Vortex
{
	using Assetmap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

		virtual Ref<Asset> GetAsset(AssetHandle handle) const = 0;
	};
}
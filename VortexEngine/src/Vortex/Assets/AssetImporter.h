#pragma once

#include "Asset.h"
#include "AssetMetaData.h"

namespace Vortex
{
	class AssetImporter
	{
	public:

		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetaData& metadata);
	};
}
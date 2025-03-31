#pragma once

#include "AssetManagerBase.h"
#include "AssetMetaData.h"

#include <map>

namespace Vortex
{
	using AssetRegistry = std::map<AssetHandle, AssetMetaData>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		const AssetMetaData& GetMetaData(AssetHandle handle) const;
		
		virtual Ref<Asset> GetAsset(AssetHandle handle) const override;

	private:
		AssetRegistry m_AssetRegistry;
		Assetmap m_LoadedAssets;

	};
}
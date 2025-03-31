#include "Vxpch.h"

#include "AssetManager.h"
#include "EditorAssetManager.h"

namespace Vortex
{
	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	const AssetMetaData& EditorAssetManager::GetMetaData(AssetHandle handle) const
	{
		static AssetMetaData s_nullMetaData;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_nullMetaData;

		return it->second;
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle)) return nullptr;

		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			const AssetMetaData& metaData = GetMetaData(handle);
			//asset = AssetImporter::ImportAsset(metaData); //Implement Asset Importer
			
			if (!asset)
				VX_CORE_ASSERT(false, "Asset Could not be Imported");
		}

		return asset;
	}


}


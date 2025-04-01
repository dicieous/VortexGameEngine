#include "Vxpch.h"
#include "AssetImporter.h"
#include "TextureImporter.h"

#include <map>

namespace Vortex
{
	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetaData&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{AssetType::Texture2D, TextureImporter::ImportTexture2D }
	};

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetaData& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			VX_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}
}

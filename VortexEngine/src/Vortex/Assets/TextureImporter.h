#pragma once

#include "Asset.h"
#include "AssetMetaData.h"
#include "Vortex/Renderer/Texture.h"

namespace Vortex
{
	class TextureImporter
	{
	public:

		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetaData& metadata);
	};

}

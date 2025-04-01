#include "Vxpch.h"
#include "TextureImporter.h"

#include <stb_image.h>

namespace Vortex
{
	Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetaData& metadata)
	{
		VX_PROFILE_FUNCTION();

		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);
		Buffer data;

		{
			std::string pathStr = metadata.FilePath.string();
			VX_PROFILE_SCOPE("stbi_load - TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetaData& metadata)");
			data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 0);
		}

		if (data.Data == nullptr)
		{
			VX_CORE_ERROR("Failed to Load Texture form FilePath: {}", metadata.FilePath.string());
			return nullptr;
		}

		//This Size Value could produce bugs
		data.Size = width * height * channels;

		TextureSpecifications specs;
		specs.Width = width;
		specs.Height = height;
		switch (channels)
		{
		case 3:
			specs.Format = ImageFormat::RGB8;
			break;

		case 4:
			specs.Format = ImageFormat::RGBA8;
			break;
		}

		//implement stbi_free_image
		auto texture = Texture2D::Create(specs, data);
		data.Release();
		return texture;
	}
}
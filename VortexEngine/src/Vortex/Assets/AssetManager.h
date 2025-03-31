#pragma once

#include "Vortex/Core/Core.h"
#include "AssetManagerBase.h"

#include "Vortex/Project/Project.h"


namespace Vortex
{
	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			auto asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}
	};
}
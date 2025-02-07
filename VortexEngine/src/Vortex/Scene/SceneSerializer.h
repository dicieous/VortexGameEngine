#pragma once

#include "Vortex/Core/Core.h"
#include "Scene.h"

namespace Vortex
{
	class SceneSerializer{

	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filePath); //Serialize in TextFile
		void SerializeRuntime(const std::string& filePath); //Serialize in Binary

		bool Deserialize(const std::string& filePath); //Deserialize in TextFile
		bool DeserializeRuntime(const std::string& filePath); //Deserialize in Binary

	private:
		Ref<Scene> m_Scene;

	};
}
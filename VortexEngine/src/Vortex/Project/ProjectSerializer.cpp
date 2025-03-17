#include "Vxpch.h"
#include "ProjectSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Vortex
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{

	}


	bool ProjectSerializer::Serialize(const std::filesystem::path& filePath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; //RootProject
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;

				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				//out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();

				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}
		std::ofstream fout(filePath);
		fout << out.c_str();

		return true;
	}


	bool ProjectSerializer::DeSerialize(const std::filesystem::path& filePath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filePath.string());
		}
		catch (YAML::ParserException e)
		{
			VX_CORE_ERROR("Failed to load Project File {0}\n {1} ", filePath.string(), e.what());
			return false;
		}


		auto projectNode = data["Project"];
		if (!projectNode)
		{
			VX_CORE_ERROR("No Project File Serialized");
			return false;
		}
		
		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		//config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		return true;
	}
}

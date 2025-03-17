#include "Vxpch.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace Vortex
{
	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		return s_ActiveProject;
	}

	Ref<Project> Vortex::Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.DeSerialize(path))
		{
			s_ActiveProject = project;
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		return serializer.Serialize(path);
	}

}

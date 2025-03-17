#include "Vxpch.h"
#include "SceneSerializer.h"
#include "Vortex/Project/Project.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#define KEY(x) << YAML::Key << x
#define VALUE(x) << YAML::Value << x
#define KEYVAL(x,y) KEY(x)VALUE(y)

#define STARTMAP << YAML::BeginMap
#define ENDMAP << YAML::EndMap

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);

			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Vortex
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string RigidBody2DBodyTypeToString(RigidBody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case RigidBody2DComponent::BodyType::Static:
			return "Static";

		case RigidBody2DComponent::BodyType::Dynamic:
			return "Dynamic";

		case RigidBody2DComponent::BodyType::Kinematic:
			return "Kinematic";
		}

		VX_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static RigidBody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static") return RigidBody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic") return RigidBody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;

		VX_CORE_ASSERT(false, "Unknown body type");
		return RigidBody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		:m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::Newline;
		out STARTMAP; //Entity
		out KEYVAL("Entity", entity.GetUUID());

		if (entity.HasComponent<TagComponent>())
		{
			out KEY("TagComponent");
			out STARTMAP; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out KEYVAL("Tag", tag);

			out ENDMAP; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out KEY("TransformComponent");
			out STARTMAP; //TransformComponent

			auto& tranformComp = entity.GetComponent<TransformComponent>();

			out KEYVAL("Translation", tranformComp.Translation);
			out KEYVAL("Rotation", tranformComp.Rotation);
			out KEYVAL("Scale", tranformComp.Scale);

			out ENDMAP; //TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out KEY("CameraComponent");
			out STARTMAP; //CameraComponent

			auto& cameraComp = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComp.Camera;

			//SceneCamera
			camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective
				? out KEYVAL("ProjectionType", "Perspective")
				: out KEYVAL("ProjectionType", "Orthographic");

			out KEYVAL("OrthographicSize", camera.GetOrthographicSize());
			out KEYVAL("OrthographicNearClip", camera.GetOrthographicNearClip());
			out KEYVAL("OrthographicFarClip", camera.GetOrthographicFarClip());

			out KEYVAL("PerspectiveFOV", camera.GetPerspectiveVerticalFOV());
			out KEYVAL("PerspectiveFarClip", camera.GetPerspectiveFarClip());
			out KEYVAL("PerspectiveNearClip", camera.GetPerspectiveNearClip());

			//cameraComponent
			out KEYVAL("Primary Camera", cameraComp.primary);
			out KEYVAL("Fixed AspectRatio", cameraComp.FixedAspectRatio);

			out ENDMAP; //CameraComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out KEY("SpriteRendererComponent");
			out STARTMAP;//SpriteRenderer

			auto& spriteComp = entity.GetComponent<SpriteRendererComponent>();
			out KEYVAL("Color", spriteComp.Color);
			if (spriteComp.Texture)
				out KEYVAL("TexturePath", spriteComp.Texture->GetPath());
			out KEYVAL("TilingFactor", spriteComp.TilingFactor);

			out ENDMAP;//SpriteRenderer
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out KEY("CircleRendererComponent");
			out STARTMAP;//CircleRenderer

			auto& circleComp = entity.GetComponent<CircleRendererComponent>();
			out KEYVAL("Color", circleComp.Color);
			out KEYVAL("Thickness", circleComp.Thickness);
			out KEYVAL("Fade", circleComp.Fade);

			out ENDMAP;//CircleRenderer
		}

		if (entity.HasComponent<RigidBody2DComponent>())
		{
			out KEY("RigidBody2DComponent");
			out STARTMAP;//RigidBody2D

			auto& rb2DComp = entity.GetComponent<RigidBody2DComponent>();

			out KEYVAL("BodyType", RigidBody2DBodyTypeToString(rb2DComp.Type));
			out KEYVAL("Fixed Rotation", rb2DComp.FixedRotation);
			out ENDMAP;//RigidBody2D
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out KEY("BoxCollider2DComponent");
			out STARTMAP;//BoxCollider2D

			auto& bc2DComp = entity.GetComponent<BoxCollider2DComponent>();

			out KEYVAL("Offset", bc2DComp.Offset);
			out KEYVAL("Size", bc2DComp.Size);

			out KEYVAL("Density", bc2DComp.Density);
			out KEYVAL("Friction", bc2DComp.Friction);
			out KEYVAL("Restitution", bc2DComp.Restitution);
			out KEYVAL("RestitutionThreshold", bc2DComp.RestitutionThreshold);

			out ENDMAP;//BoxCollider2D
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out KEY("CircleCollider2DComponent");
			out STARTMAP;//CircleCollider2D

			auto& cc2DComp = entity.GetComponent<CircleCollider2DComponent>();

			out KEYVAL("Offset", cc2DComp.Offset);
			out KEYVAL("Radius", cc2DComp.Radius);

			out KEYVAL("Density", cc2DComp.Density);
			out KEYVAL("Friction", cc2DComp.Friction);
			out KEYVAL("Restitution", cc2DComp.Restitution);
			out KEYVAL("RestitutionThreshold", cc2DComp.RestitutionThreshold);

			out ENDMAP;//CircleCollider2D
		}

		out ENDMAP; //Entity
	}

	void SceneSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter out;
		out STARTMAP;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.view<entt::entity>().each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out ENDMAP;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filePath)
	{
		//Not implemented
		VX_CORE_ASSERT(false, "Not Implemented Yet");
	}

	bool SceneSerializer::Deserialize(const std::string& filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream str;
		str << stream.rdbuf();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filePath);
		}
		catch (YAML::ParserException e)
		{
			VX_CORE_ERROR("Failed to load .vortex File {0}\n {1} ", filePath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		VX_CORE_TRACE("Deserialized Scene '{0}' ", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			std::vector<YAML::Node> entityList;
			for (auto entity : entities)
				entityList.push_back(entity);

			for (auto it = entityList.rbegin(); it != entityList.rend(); ++it)
			{
				auto& entity = *it;
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				VX_CORE_TRACE("Deserialized Entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					//Contains Transform by Default
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto spriteComponent = entity["SpriteRendererComponent"];
				if (spriteComponent)
				{
					auto& spriteC = deserializedEntity.AddComponent<SpriteRendererComponent>();
					spriteC.Color = spriteComponent["Color"].as<glm::vec4>();
					if (spriteComponent["TexturePath"])
					{
						std::string texturePath = spriteComponent["TexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						spriteC.Texture = Texture2D::Create(path.string());
					}

					if (spriteComponent["TilingFactor"])
						spriteC.TilingFactor = spriteComponent["TilingFactor"].as<float>();
				}

				auto circleComponent = entity["CircleRendererComponent"];
				if (circleComponent)
				{
					auto& circleC = deserializedEntity.AddComponent<CircleRendererComponent>();
					circleC.Color = circleComponent["Color"].as<glm::vec4>();
					circleC.Thickness = circleComponent["Thickness"].as<float>();
					circleC.Fade = circleComponent["Fade"].as<float>();
				}

				auto rb2DComponent = entity["RigidBody2DComponent"];
				if (rb2DComponent)
				{
					auto& rb2DC = deserializedEntity.AddComponent<RigidBody2DComponent>();
					rb2DC.Type = RigidBody2DBodyTypeFromString(rb2DComponent["BodyType"].as<std::string>());
					rb2DC.FixedRotation = rb2DComponent["Fixed Rotation"].as<bool>();
				}

				auto bc2DComponent = entity["BoxCollider2DComponent"];
				if (bc2DComponent)
				{
					auto& bc2DC = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2DC.Offset = bc2DComponent["Offset"].as<glm::vec2>();
					bc2DC.Size = bc2DComponent["Size"].as<glm::vec2>();

					bc2DC.Density = bc2DComponent["Density"].as<float>();
					bc2DC.Friction = bc2DComponent["Friction"].as<float>();
					bc2DC.Restitution = bc2DComponent["Restitution"].as<float>();
					bc2DC.RestitutionThreshold = bc2DComponent["RestitutionThreshold"].as<float>();
				}

				auto cc2DComponent = entity["CircleCollider2DComponent"];
				if (cc2DComponent)
				{
					auto& cc2DC = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2DC.Offset = cc2DComponent["Offset"].as<glm::vec2>();
					cc2DC.Radius = cc2DComponent["Radius"].as<float>();

					cc2DC.Density = cc2DComponent["Density"].as<float>();
					cc2DC.Friction = cc2DComponent["Friction"].as<float>();
					cc2DC.Restitution = cc2DComponent["Restitution"].as<float>();
					cc2DC.RestitutionThreshold = cc2DComponent["RestitutionThreshold"].as<float>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cameraC = deserializedEntity.AddComponent<CameraComponent>();
					cameraC.Camera.SetProjectionType(cameraComponent["ProjectionType"].as<std::string>() == "Perspective"
						? SceneCamera::ProjectionType::Perspective
						: SceneCamera::ProjectionType::Orthographic);

					cameraC.Camera.SetOrthographicSize(cameraComponent["OrthographicSize"].as<float>());
					cameraC.Camera.SetOrthoGraphicFarClip(cameraComponent["OrthographicFarClip"].as<float>());
					cameraC.Camera.SetOrthoGraphicNearClip(cameraComponent["OrthographicNearClip"].as<float>());

					cameraC.Camera.SetPerspectiveVerticalFOV(cameraComponent["PerspectiveFOV"].as<float>());
					cameraC.Camera.SetPerspectiveFarClip(cameraComponent["PerspectiveFarClip"].as<float>());
					cameraC.Camera.SetPerspectiveNearClip(cameraComponent["PerspectiveNearClip"].as<float>());


					cameraC.primary = cameraComponent["Primary Camera"].as<bool>();
					cameraC.FixedAspectRatio = cameraComponent["Fixed AspectRatio"].as<bool>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		//Not implemented
		VX_CORE_ASSERT(false, "Not Implemented Yet");
		return false;
	}
}
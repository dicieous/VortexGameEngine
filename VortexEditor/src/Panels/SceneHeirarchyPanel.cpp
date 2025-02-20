#include "SceneHeirarchyPanel.h"
#include "Vortex/Scene/Components.h"
#include "Vortex/Renderer/Texture.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace Vortex
{
	extern const std::filesystem::path g_AssetsPath;

	SceneHeirarchyPanel::SceneHeirarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHeirarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHeirarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Heirarchy");
		if (m_Context)
		{
			m_Context->m_Registry.view<entt::entity>().each([&](auto entityID)
				{
					Entity entity{ entityID, m_Context.get() };
					DrawEntityNode(entity);
				});

			if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			//Right-Click on Black Space
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Entity"))
				{
					m_Context->CreateEntity("Empty Entity");
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();


		ImGui::Begin("Properties");

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHeirarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHeirarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("delete Entity"))
			{
				entityDeleted = true;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			//Can Render Child Nodes Here
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			if (m_SelectionContext == entity) m_SelectionContext = {};
			m_Context->DestroyEntity(entity);
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValues = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldfont = io.Fonts->Fonts[0]; //Make a Font Loading System(if needed more fonts)

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f,lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldfont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValues;
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldfont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValues;
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldfont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValues;
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}


	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionavailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionavailable.x - lineHeight * 0.5f);
			if (ImGui::Button(":", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
				{
					removeComponent = true;
				}
				ImGui::EndPopup();
			}


			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}
	}

	void SceneHeirarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Position", component.Translation);

				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);

				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.primary);

				const char* projectionTypeStrings[] = { "Perspective", "OrthoGraphic" };

				const char* currentProjectionType = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionType))
				{
					for (int type = 0; type < 2; type++)
					{
						bool isSelected = currentProjectionType == projectionTypeStrings[type];
						if (ImGui::Selectable(projectionTypeStrings[type], isSelected))
						{
							currentProjectionType = projectionTypeStrings[type];
							camera.SetProjectionType((SceneCamera::ProjectionType)type);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float verticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &verticalFOV))
					{
						camera.SetPerspectiveVerticalFOV(glm::radians(verticalFOV));
					}

					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("NearClip", &nearClip))
					{
						camera.SetPerspectiveNearClip(nearClip);
					}

					float farClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("FarClip", &farClip))
					{
						camera.SetPerspectiveFarClip(farClip);
					}
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					ImGui::Checkbox("Fixed AspectRatio", &component.FixedAspectRatio);

					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
					{
						camera.SetOrthographicSize(orthoSize);
					}

					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("NearClip", &nearClip))
					{
						camera.SetOrthoGraphicNearClip(nearClip);
					}

					float farClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("FarClip", &farClip))
					{
						camera.SetOrthoGraphicFarClip(farClip);
					}

				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payLoad->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetsPath) / path;
						component.Texture = Texture2D::Create(texturePath.string());
					}

					ImGui::EndDragDropTarget();
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});


		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});


		DrawComponent<RigidBody2DComponent>("RigidBody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };

				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int type = 0; type < 3; type++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[type];
						if (ImGui::Selectable(bodyTypeStrings[type], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[type];
							component.Type = (RigidBody2DComponent::BodyType)type;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});


		DrawComponent<BoxCollider2DComponent>("BoxCollider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));

				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleCollider2DComponent>("CircleCollider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius);

				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});


		ImGui::Separator();

		ImGui::Dummy(ImVec2(0.0f, 4.0f));


		float panelWidth = ImGui::GetWindowWidth();
		float buttonWidth = panelWidth * 0.5f;

		// Center the button
		ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

		if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0)))
			ImGui::OpenPopup("AddComponent");

		ImGui::PopStyleVar();

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!m_SelectionContext.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					m_SelectionContext.AddComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<RigidBody2DComponent>())
			{
				if (ImGui::MenuItem("RigidBody 2D"))
				{
					m_SelectionContext.AddComponent<RigidBody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::MenuItem("BoxCollider 2D"))
				{
					m_SelectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<CircleCollider2DComponent>())
			{
				if (ImGui::MenuItem("CircleCollider 2D"))
				{
					m_SelectionContext.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			
			ImGui::EndPopup();
			

		}

		ImGui::Dummy(ImVec2(0.0f, 100.0f));
	}
}
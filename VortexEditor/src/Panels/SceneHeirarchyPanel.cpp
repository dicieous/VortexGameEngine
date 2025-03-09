#include "SceneHeirarchyPanel.h"
#include "Vortex/Scene/Components.h"
#include "Vortex/Renderer/Texture.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace Vortex
{
	static bool BeginTwoTableContent(ImGuiTableFlags flags = ImGuiTableFlags_None)
	{
		bool table = ImGui::BeginTable("layout_table", 2, flags | ImGuiTableFlags_SizingStretchProp);
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
		ImGui::TableSetupColumn("Widget");

		return table;
	}

	static void EndTwoTableContent()
	{
		ImGui::EndTable();
	}

	template<typename Func>
	static bool LabeledUI(const char* label, Func imguiFunc)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		// Get row height
		float rowHeight = ImGui::GetTextLineHeightWithSpacing();
		float cursorY = ImGui::GetCursorPosY();

		// Center the text vertically
		ImGui::SetCursorPosY(cursorY + (rowHeight - ImGui::GetTextLineHeight()) * 0.5f);
		ImGui::Text(label);

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-FLT_MIN);
		if constexpr (std::is_same_v<decltype(imguiFunc()), void>)// Checks if the function got a void return type of bool
		{
			imguiFunc();
			return false;
		}
		else
		{
			return imguiFunc();
		}
		ImGui::PopItemWidth();
	}


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
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		// Begin table with 2 columns
		if (ImGui::BeginTable("Vec3Table", 2, ImGuiTableFlags_NoBordersInBody))
		{
			// Set first column width
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableSetupColumn("Values", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			// Add label and vertical separator
			ImGui::PushFont(boldFont);
			ImGui::SetWindowFontScale(1.1f);
			ImGui::Text(label.c_str());
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopFont();
			//ImGui::SameLine(columnWidth - 5.0f); // Adjust separator position
			//ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "|");

			ImGui::TableSetColumnIndex(1);

			// Calculate widths for even distribution
			float availableWidth = ImGui::GetContentRegionAvail().x;
			float componentWidth = availableWidth / 3.0f - ImGui::GetStyle().ItemSpacing.x;

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			// Adjust spacing between button and value
			const float tightSpacing = 2.0f;

			// X Component
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
				values.x = resetValues;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine(0.0f, tightSpacing);
			ImGui::SetNextItemWidth(componentWidth - buttonSize.x - tightSpacing);
			ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");

			ImGui::SameLine();

			// Y Component
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
				values.y = resetValues;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine(0.0f, tightSpacing);
			ImGui::SetNextItemWidth(componentWidth - buttonSize.x - tightSpacing);
			ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");

			ImGui::SameLine();

			// Z Component
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", buttonSize))
				values.z = resetValues;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine(0.0f, tightSpacing);
			ImGui::SetNextItemWidth(componentWidth - buttonSize.x - tightSpacing);
			ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");

			ImGui::EndTable();
		}

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
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.12f, 0.125f, 0.12f, 1.0f });
			if (ImGui::Button(":", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleColor();

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

			// Get row height
			float rowHeight = ImGui::GetTextLineHeightWithSpacing();
			float cursorY = ImGui::GetCursorPosY();

			// Center the text vertically
			ImGui::SetCursorPosY(cursorY + (rowHeight - ImGui::GetTextLineHeight()) * 0.5f);
			ImGui::Text("Entity Name");

			ImGui::SameLine();
			ImGui::SetCursorPosY(cursorY);
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
				ImGuiTableFlags flags = ImGuiTableFlags_None;
				if (BeginTwoTableContent(flags))
				{
					auto& camera = component.Camera;

					LabeledUI("Primary", [&]() { ImGui::Checkbox("##Primary", &component.primary); });

					const char* projectionTypeStrings[] = { "Perspective", "OrthoGraphic" };

					LabeledUI("Projection", [&]()
						{
							const char* currentProjectionType = projectionTypeStrings[(int)camera.GetProjectionType()];
							if (ImGui::BeginCombo("##Projection", currentProjectionType))
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
						});

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
					{
						LabeledUI("Vertical FOV", [&]()
							{
								float verticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
								if (ImGui::DragFloat("##Vertical FOV", &verticalFOV))
								{
									camera.SetPerspectiveVerticalFOV(glm::radians(verticalFOV));
								}
							});

						LabeledUI("NearClip", [&]()
							{
								float nearClip = camera.GetPerspectiveNearClip();
								if (ImGui::DragFloat("##NearClip", &nearClip))
								{
									camera.SetPerspectiveNearClip(nearClip);
								}
							});

						LabeledUI("FarClip", [&]()
							{
								float farClip = camera.GetPerspectiveFarClip();
								if (ImGui::DragFloat("##FarClip", &farClip))
								{
									camera.SetPerspectiveFarClip(farClip);
								}
							});
					}

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
					{
						LabeledUI("Fixed AspectRatio", [&]()
							{
								ImGui::Checkbox("##Fixed AspectRatio", &component.FixedAspectRatio);
							});

						LabeledUI("Size", [&]()
							{
								float orthoSize = camera.GetOrthographicSize();
								if (ImGui::DragFloat("##Size", &orthoSize))
								{
									camera.SetOrthographicSize(orthoSize);
								}
							});

						LabeledUI("NearClip", [&]()
							{
								float nearClip = camera.GetOrthographicNearClip();
								if (ImGui::DragFloat("##NearClip", &nearClip))
								{
									camera.SetOrthoGraphicNearClip(nearClip);
								}
							});

						LabeledUI("FarClip", [&]()
							{
								float farClip = camera.GetOrthographicFarClip();
								if (ImGui::DragFloat("##FarClip", &farClip))
								{
									camera.SetOrthoGraphicFarClip(farClip);
								}
							});
					}

					EndTwoTableContent();
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGuiTableFlags flags = ImGuiTableFlags_None;
				if (BeginTwoTableContent(flags))
				{

					ImVec2 textureSlotSize = { 100.0f, 100.0f };
					ImTextureID textureID = (component.Texture) ? component.Texture->GetRendererID() : -1;

#if 0
					//Don't know why it's not working(sit on it when free)
					if (!component.Texture)
					{
						auto greyTexture = Texture2D::Create(1, 1);
						uint8_t grayTextureData[4] = { 192, 192, 192, 255 }; // R, G, B, A
						greyTexture->SetData(grayTextureData, sizeof(grayTextureData));
						textureID = greyTexture->GetRendererID();
					}
#endif

					LabeledUI("Sprite: ", [&]()
						{
							ImGui::ImageButton("##SpriteCircleTextureSlot", textureID, textureSlotSize, { 0, 1 }, { 1, 0 });
						});

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						component.Texture = nullptr;
					}

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

					LabeledUI("Color", [&]()
						{
							ImGuiColorEditFlags colorflags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_PickerHueWheel;
							ImGui::ColorEdit4("##Color", glm::value_ptr(component.Color), colorflags);
						});

					LabeledUI("Tiling Factor", [&]() { ImGui::DragFloat("##Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f); });

					EndTwoTableContent();
				}
			});


		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				if (BeginTwoTableContent())
				{
					LabeledUI("Color", [&]()
						{
							ImGuiColorEditFlags colorflags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_PickerHueWheel;
							ImGui::ColorEdit4("##Color", glm::value_ptr(component.Color), colorflags);
						});

					LabeledUI("Thickness", [&]() { ImGui::DragFloat("##Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_ClampOnInput); });
					LabeledUI("Fade", [&]() { ImGui::DragFloat("##Fade", &component.Fade, 0.00025f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_ClampOnInput); });

					EndTwoTableContent();
				}
			});


		DrawComponent<RigidBody2DComponent>("RigidBody 2D", entity, [](auto& component)
			{
				if (BeginTwoTableContent())
				{
					const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };

					LabeledUI("Body Type", [&]()
						{
							const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
							if (ImGui::BeginCombo("##Body Type", currentBodyTypeString))
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
						});

					LabeledUI("Fixed Rotation", [&]() { ImGui::Checkbox("##Fixed Rotation", &component.FixedRotation); });

					EndTwoTableContent();
				}
			});


		DrawComponent<BoxCollider2DComponent>("BoxCollider 2D", entity, [](auto& component)
			{
				if (BeginTwoTableContent())
				{
					LabeledUI("Offset", [&]() { ImGui::DragFloat2("##Offset", glm::value_ptr(component.Offset)); });
					LabeledUI("Size", [&]() { ImGui::DragFloat2("##Size", glm::value_ptr(component.Size)); });
					
					ImGui::Dummy(ImVec2(0.0f, 5.0f));
					
					LabeledUI("Density", [&]() { ImGui::DragFloat("##Density", &component.Density, 0.01f, 0.0f, 1.0f); });
					LabeledUI("Friction", [&]() { ImGui::DragFloat("##Friction", &component.Friction, 0.01f, 0.0f, 1.0f); });
					LabeledUI("Restitution", [&]() { ImGui::DragFloat("##Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f); });
					LabeledUI("RestitutionThreshold", [&]() { ImGui::DragFloat("##RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f); });

					EndTwoTableContent();
				}
			});

		DrawComponent<CircleCollider2DComponent>("CircleCollider 2D", entity, [](auto& component)
			{
				if (BeginTwoTableContent())
				{
					LabeledUI("Offset", [&]() { ImGui::DragFloat2("##Offset", glm::value_ptr(component.Offset), 0.1f, 0.0f); });
					LabeledUI("Radius", [&]() { ImGui::DragFloat("##Radius", &component.Radius, 0.1f, 0.0f); });

					ImGui::Dummy(ImVec2(0.0f, 5.0f));

					LabeledUI("Density", [&]() { ImGui::DragFloat("##Density", &component.Density, 0.01f, 0.0f, 1.0f); });
					LabeledUI("Friction", [&]() { ImGui::DragFloat("##Friction", &component.Friction, 0.01f, 0.0f, 1.0f); });
					LabeledUI("Restitution", [&]() { ImGui::DragFloat("##Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f); });
					LabeledUI("RestitutionThreshold", [&]() { ImGui::DragFloat("##RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f); });

					EndTwoTableContent();
				}
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
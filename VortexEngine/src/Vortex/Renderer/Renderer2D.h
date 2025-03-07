#pragma once
#include "OrthographicCamera.h"
#include "Texture.h"
#include "SubTexture2D.h"
#include "Camera.h"
#include "EditorCamera.h"

#include "Vortex/Scene/Components.h"

namespace Vortex {

	struct InfiniteGridSpecifications
	{
		//Make it so that these values can be manipulated from the editor
		bool Enabled = true;
		float Size = 500.0f;
		float CellSize = 0.5f;
		glm::vec4 ColorThin = glm::vec4(0.5, 0.5, 0.5, 0.25);
		glm::vec4 ColorThick = glm::vec4(0.0, 0.0, 0.0, 0.25);
		float MinPixelsBetweenCells = 2.0f;
	};

	class Renderer2D {

	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);

		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Flush();

		//TODO: Make it Clean
		//primitives////
		//Quads
		static void DrawQuads(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuads(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuads(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		static void DrawQuads(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		
		static void DrawQuads(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuads(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f }, int entityID = -1);

		//Rotation is in radians
		static void DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		static void DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });

		//Circles
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		//Lines
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);
		void SetLineWidth(float width);
		float GetLineWidth() const;

		//Rect
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);

		//InfiniteGrid
		static void DrawInfiniteGrid(const InfiniteGridSpecifications& gridSpecs = InfiniteGridSpecifications());

		//Editor-Only
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID);

		//Statistics
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();

	private:

		static void FlushAndReset();
	};
}
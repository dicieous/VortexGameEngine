#pragma once
#include "OrthographicCamera.h"
#include "Texture.h"
#include "SubTexture2D.h"
#include "Camera.h"
#include "EditorCamera.h"

namespace Vortex {

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
		static void DrawQuads(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuads(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		
		static void DrawQuads(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuads(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuads(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		static void DrawQuads(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		
		static void DrawQuads(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		static void DrawQuads(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });

		//Rotation is in radians
		static void DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);

		static void DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		static void DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		
		static void DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });
		static void DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f,1.0f,1.0f,1.0f });

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
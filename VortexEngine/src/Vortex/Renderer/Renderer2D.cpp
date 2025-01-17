#include "Vxpch.h"
#include "Renderer2D.h"

#include "VertexArray.h";
#include "Shader.h";
#include "RenderCommand.h"

#include <glm/ext/matrix_transform.hpp>

namespace Vortex {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2Ddata
	{
		static const uint32_t MAX_QUADS = 10000;
		static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
		static const uint32_t MAX_INDICES = MAX_QUADS * 6;
		static const uint32_t MAX_TEXTURE_SLOTS = 32; // TODO: Render Capabilities

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase;
		QuadVertex* QuadVertexBufferPtr;

		std::array<Ref<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white Texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};

	static Renderer2Ddata s_2Ddata;

	void Renderer2D::Init()
	{
		VX_PROFILE_FUNCTION();

		s_2Ddata.QuadVertexArray = VertexArray::Create();

		s_2Ddata.QuadVertexBuffer = VertexBuffer::Create(s_2Ddata.MAX_VERTICES * sizeof(QuadVertex));

		s_2Ddata.QuadVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TilingFactor"}
			});

		s_2Ddata.QuadVertexArray->AddVertexBuffer(s_2Ddata.QuadVertexBuffer);

		s_2Ddata.QuadVertexBufferBase = new QuadVertex[s_2Ddata.MAX_VERTICES];

		uint32_t* quadIndices = new uint32_t[s_2Ddata.MAX_INDICES];

		uint32_t offset = 0;
		for (int i = 0; i < s_2Ddata.MAX_INDICES; i += 6) {

			if (i + 5 >= s_2Ddata.MAX_INDICES) break;

			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_2Ddata.MAX_INDICES);
		s_2Ddata.QuadVertexArray->SetIndexBuffer(quadIB);

		delete[] quadIndices;

		s_2Ddata.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_2Ddata.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int samplers[s_2Ddata.MAX_TEXTURE_SLOTS];

		for (uint32_t i = 0; i < s_2Ddata.MAX_TEXTURE_SLOTS; i++) {
			samplers[i] = i;
		}

		s_2Ddata.TextureShader = Shader::Create("Assets/Shaders/Texture.shader");
		s_2Ddata.TextureShader->Bind();
		s_2Ddata.TextureShader->SetIntArray("u_Texture", samplers, s_2Ddata.MAX_TEXTURE_SLOTS);

		s_2Ddata.TextureSlots[0] = s_2Ddata.WhiteTexture;

		s_2Ddata.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		VX_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		VX_PROFILE_FUNCTION();

		s_2Ddata.TextureShader->Bind();
		s_2Ddata.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_2Ddata.QuadIndexCount = 0;
		s_2Ddata.QuadVertexBufferPtr = s_2Ddata.QuadVertexBufferBase;

		s_2Ddata.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		VX_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_2Ddata.QuadVertexBufferPtr - (uint8_t*)s_2Ddata.QuadVertexBufferBase;
		s_2Ddata.QuadVertexBuffer->SetData(s_2Ddata.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		//Bind Textures
		for (uint32_t i = 0; i < s_2Ddata.TextureSlotIndex; i++) {
			s_2Ddata.TextureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_2Ddata.QuadVertexArray, s_2Ddata.QuadIndexCount);

		s_2Ddata.Stats.DrawCalls++;
	}

	void Renderer2D::FlushAndReset() {

		EndScene();

		s_2Ddata.QuadIndexCount = 0;
		s_2Ddata.QuadVertexBufferPtr = s_2Ddata.QuadVertexBufferBase;

		s_2Ddata.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		const float texIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y, 1.0f });

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[0];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[1];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[2];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[3];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}


	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_2Ddata.TextureSlotIndex; i++)
		{
			if (*s_2Ddata.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_2Ddata.TextureSlotIndex;
			s_2Ddata.TextureSlots[s_2Ddata.TextureSlotIndex] = texture;
			s_2Ddata.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y, 1.0f });

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[0];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[1];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[2];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[3];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuads({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		const float texIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y, 1.0f });


		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[0];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[1];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[2];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[3];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuads({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_2Ddata.TextureSlotIndex; i++)
		{
			if (*s_2Ddata.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_2Ddata.TextureSlotIndex;
			s_2Ddata.TextureSlots[s_2Ddata.TextureSlotIndex] = texture;
			s_2Ddata.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[0];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[1];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[2];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[3];
		s_2Ddata.QuadVertexBufferPtr->color = color;
		s_2Ddata.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_2Ddata.QuadVertexBufferPtr++;

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_2Ddata.Stats;
	}

	void Renderer2D::ResetStats()
	{
		s_2Ddata.Stats = Statistics();
		//memset(&s_2Ddata.Stats, 0, sizeof(Statistics));
	}
}

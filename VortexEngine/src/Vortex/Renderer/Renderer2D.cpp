
#include "Vxpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

namespace Vortex {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		//Editor-Only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		//Editor-Only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 color;

		//Editor-Only
		int EntityID;
	};

	struct Renderer2Ddata
	{
		static const uint32_t MAX_QUADS = 10000;
		static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
		static const uint32_t MAX_INDICES = MAX_QUADS * 6;
		static const uint32_t MAX_TEXTURE_SLOTS = 32; // TODO: Render Capabilities

		//Quads
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		//Circles
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		//Line
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		float LineWidth = 2.0f;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white Texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};

	static Renderer2Ddata s_2Ddata;

	void Renderer2D::Init()
	{
		VX_PROFILE_FUNCTION();
		//Quads
		s_2Ddata.QuadVertexArray = VertexArray::Create();

		s_2Ddata.QuadVertexBuffer = VertexBuffer::Create(s_2Ddata.MAX_VERTICES * sizeof(QuadVertex));

		s_2Ddata.QuadVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"	  },
			{ShaderDataType::Float4, "a_Color"		  },
			{ShaderDataType::Float2, "a_TexCoord"	  },
			{ShaderDataType::Float,	 "a_TexIndex"	  },
			{ShaderDataType::Float,	 "a_TilingFactor" },
			{ShaderDataType::Int,	 "a_EntityID"     },
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

		//Circles
		s_2Ddata.CircleVertexArray = VertexArray::Create();

		s_2Ddata.CircleVertexBuffer = VertexBuffer::Create(s_2Ddata.MAX_VERTICES * sizeof(CircleVertex));

		s_2Ddata.CircleVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_WorldPosition" },
			{ShaderDataType::Float3, "a_LocalPosition" },
			{ShaderDataType::Float4, "a_Color"		   },
			{ShaderDataType::Float,	 "a_Thickness"	   },
			{ShaderDataType::Float,	 "a_Fade"		   },
			{ShaderDataType::Int,	 "a_EntityID"      },
			});

		s_2Ddata.CircleVertexArray->AddVertexBuffer(s_2Ddata.CircleVertexBuffer);
		s_2Ddata.CircleVertexArray->SetIndexBuffer(quadIB); //Used QuadIB
		s_2Ddata.CircleVertexBufferBase = new CircleVertex[s_2Ddata.MAX_VERTICES];

		//Lines
		s_2Ddata.LineVertexArray = VertexArray::Create();

		s_2Ddata.LineVertexBuffer = VertexBuffer::Create(s_2Ddata.MAX_VERTICES * sizeof(LineVertex));

		s_2Ddata.LineVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"	  },
			{ShaderDataType::Float4, "a_Color"		  },
			{ShaderDataType::Int,	 "a_EntityID"     },
			});

		s_2Ddata.LineVertexArray->AddVertexBuffer(s_2Ddata.LineVertexBuffer);
		s_2Ddata.LineVertexBufferBase = new LineVertex[s_2Ddata.MAX_VERTICES];


		s_2Ddata.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_2Ddata.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int samplers[s_2Ddata.MAX_TEXTURE_SLOTS];

		for (uint32_t i = 0; i < s_2Ddata.MAX_TEXTURE_SLOTS; i++) {
			samplers[i] = i;
		}

		s_2Ddata.QuadShader = Shader::Create("Assets/Shaders/Renderer2D_Quad.shader");
		s_2Ddata.QuadShader->Bind();
		s_2Ddata.QuadShader->SetIntArray("u_Texture", samplers, s_2Ddata.MAX_TEXTURE_SLOTS);

		s_2Ddata.CircleShader = Shader::Create("Assets/Shaders/Renderer2D_Circle.shader");
		s_2Ddata.LineShader = Shader::Create("Assets/Shaders/Renderer2D_Line.shader");
		
		s_2Ddata.TextureSlots[0] = s_2Ddata.WhiteTexture;

		s_2Ddata.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		VX_PROFILE_FUNCTION();

		delete[] s_2Ddata.QuadVertexBufferBase;
		delete[] s_2Ddata.CircleVertexBufferBase;
		delete[] s_2Ddata.LineVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		VX_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		s_2Ddata.QuadShader->Bind();
		s_2Ddata.QuadShader->SetMat4("u_ViewProjection", viewProj);
		s_2Ddata.QuadIndexCount = 0;
		s_2Ddata.QuadVertexBufferPtr = s_2Ddata.QuadVertexBufferBase;

		s_2Ddata.CircleShader->Bind();
		s_2Ddata.CircleShader->SetMat4("u_ViewProjection", viewProj);
		s_2Ddata.CircleIndexCount = 0;
		s_2Ddata.CircleVertexBufferPtr = s_2Ddata.CircleVertexBufferBase;
		
		s_2Ddata.LineShader->Bind();
		s_2Ddata.LineShader->SetMat4("u_ViewProjection", viewProj);
		s_2Ddata.LineVertexCount = 0;
		s_2Ddata.LineVertexBufferPtr = s_2Ddata.LineVertexBufferBase;

		s_2Ddata.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		glm::mat4 viewProj = camera.GetViewProjectionMatrix();

		s_2Ddata.QuadShader->Bind();
		s_2Ddata.QuadShader->SetMat4("u_ViewProjection", viewProj);
		s_2Ddata.QuadIndexCount = 0;
		s_2Ddata.QuadVertexBufferPtr = s_2Ddata.QuadVertexBufferBase;

		s_2Ddata.CircleShader->Bind();
		s_2Ddata.CircleShader->SetMat4("u_ViewProjection", viewProj);
		s_2Ddata.CircleIndexCount = 0;
		s_2Ddata.CircleVertexBufferPtr = s_2Ddata.CircleVertexBufferBase;

		s_2Ddata.LineShader->Bind();
		s_2Ddata.LineShader->SetMat4("u_ViewProjection", viewProj);
		s_2Ddata.LineVertexCount = 0;
		s_2Ddata.LineVertexBufferPtr = s_2Ddata.LineVertexBufferBase;

		s_2Ddata.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		VX_PROFILE_FUNCTION();

		s_2Ddata.QuadShader->Bind();
		s_2Ddata.QuadShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_2Ddata.QuadIndexCount = 0;
		s_2Ddata.QuadVertexBufferPtr = s_2Ddata.QuadVertexBufferBase;

		s_2Ddata.CircleShader->Bind();
		s_2Ddata.CircleShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_2Ddata.CircleIndexCount = 0;
		s_2Ddata.CircleVertexBufferPtr = s_2Ddata.CircleVertexBufferBase;

		s_2Ddata.LineShader->Bind();
		s_2Ddata.LineShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_2Ddata.LineVertexCount = 0;
		s_2Ddata.LineVertexBufferPtr = s_2Ddata.LineVertexBufferBase;

		s_2Ddata.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		VX_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_2Ddata.QuadIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_2Ddata.QuadVertexBufferPtr - (uint8_t*)s_2Ddata.QuadVertexBufferBase;
			s_2Ddata.QuadVertexBuffer->SetData(s_2Ddata.QuadVertexBufferBase, dataSize);

			//Bind Textures
			for (uint32_t i = 0; i < s_2Ddata.TextureSlotIndex; i++)
			{
				s_2Ddata.TextureSlots[i]->Bind(i);
			}

			s_2Ddata.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_2Ddata.QuadVertexArray, s_2Ddata.QuadIndexCount);
			s_2Ddata.Stats.DrawCalls++;
		}

		if (s_2Ddata.CircleIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_2Ddata.CircleVertexBufferPtr - (uint8_t*)s_2Ddata.CircleVertexBufferBase;
			s_2Ddata.CircleVertexBuffer->SetData(s_2Ddata.CircleVertexBufferBase, dataSize);

			s_2Ddata.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_2Ddata.CircleVertexArray, s_2Ddata.CircleIndexCount);
			s_2Ddata.Stats.DrawCalls++;
		}

		if (s_2Ddata.LineVertexCount)
		{
			uint32_t dataSize = (uint8_t*)s_2Ddata.LineVertexBufferPtr - (uint8_t*)s_2Ddata.LineVertexBufferBase;
			s_2Ddata.LineVertexBuffer->SetData(s_2Ddata.LineVertexBufferBase, dataSize);

			s_2Ddata.LineShader->Bind();
			RenderCommand::SetLineWidth(s_2Ddata.LineWidth);
			RenderCommand::DrawLine(s_2Ddata.LineVertexArray, s_2Ddata.LineVertexCount);
			s_2Ddata.Stats.DrawCalls++;
		}
	}


	void Renderer2D::FlushAndReset() {

		EndScene();

		s_2Ddata.QuadIndexCount = 0;
		s_2Ddata.QuadVertexBufferPtr = s_2Ddata.QuadVertexBufferBase;

		s_2Ddata.CircleIndexCount = 0;
		s_2Ddata.CircleVertexBufferPtr = s_2Ddata.CircleVertexBufferBase;

		s_2Ddata.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuads(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		const float texIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;
		const int maxVertexCount = 4;
		const glm::vec2 TexCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		for (int i = 0; i < maxVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->color = color;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = texIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr->EntityID = entityID;
			s_2Ddata.QuadVertexBufferPtr++;
		}

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuads(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		const int maxVertexCount = 4;
		const glm::vec2 TexCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

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

		for (int i = 0; i < maxVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->color = color;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr->EntityID = entityID;
			s_2Ddata.QuadVertexBufferPtr++;
		}

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		VX_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y, 1.0f });

		DrawQuads(transform, color);
	}


	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y, 1.0f });

		DrawQuads(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuads(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuads({ position.x, position.y, 0.0f }, size, subTexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuads(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		const int maxVertexCount = 4;
		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const Ref<Texture2D> texture = subTexture->GetTexture();

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


		for (int i = 0; i < maxVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->color = color;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr++;
		}

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

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;
		const int maxVertexCount = 4;
		const glm::vec2 TexCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), glm::vec3{ size.x, size.y, 1.0f });


		for (int i = 0; i < maxVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->color = color;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr++;
		}

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
		const int maxVertexCount = 4;
		const glm::vec2 TexCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

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
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		for (int i = 0; i < maxVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->color = color;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr++;
		}

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}


	void Renderer2D::DrawRotatedQuads(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuads({ position.x, position.y, 0.0f }, size, rotation, subTexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuads(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		VX_PROFILE_FUNCTION();

		if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
			FlushAndReset();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		const int maxVertexCount = 4;
		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const Ref<Texture2D> texture = subTexture->GetTexture();

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
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		for (int i = 0; i < maxVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->color = color;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr++;
		}

		s_2Ddata.QuadIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		VX_PROFILE_FUNCTION();

		//if (s_2Ddata.QuadIndexCount >= Renderer2Ddata::MAX_INDICES)
		//	FlushAndReset();
		
		const int maxVertexCount = 4;

		for (int i = 0; i < maxVertexCount; i++)
		{
			s_2Ddata.CircleVertexBufferPtr->WorldPosition = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.CircleVertexBufferPtr->LocalPosition = s_2Ddata.QuadVertexPositions[i] * 2.0f;
			s_2Ddata.CircleVertexBufferPtr->Color = color;
			s_2Ddata.CircleVertexBufferPtr->Thickness = thickness;
			s_2Ddata.CircleVertexBufferPtr->Fade = fade;
			s_2Ddata.CircleVertexBufferPtr->EntityID = entityID;
			s_2Ddata.CircleVertexBufferPtr++;
		}

		s_2Ddata.CircleIndexCount += 6;

		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_2Ddata.LineVertexBufferPtr->Position = p0;
		s_2Ddata.LineVertexBufferPtr->color = color;
		s_2Ddata.LineVertexBufferPtr->EntityID = entityID;
		s_2Ddata.LineVertexBufferPtr++;

		s_2Ddata.LineVertexBufferPtr->Position = p1;
		s_2Ddata.LineVertexBufferPtr->color = color;
		s_2Ddata.LineVertexBufferPtr->EntityID = entityID;
		s_2Ddata.LineVertexBufferPtr++;

		s_2Ddata.LineVertexCount += 2;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_2Ddata.LineWidth = width;
	}

	float Renderer2D::GetLineWidth() const
	{
		return s_2Ddata.LineWidth;
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_2Ddata.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
		DrawLine(lineVertices[0], lineVertices[2], color);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
		DrawLine(p0, p2, color);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID)
	{
		if (sprite.Texture)
			DrawQuads(transform, sprite.Texture, sprite.TilingFactor, sprite.Color, entityID);
		else
			DrawQuads(transform, sprite.Color, entityID);
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_2Ddata.Stats;
	}

	void Renderer2D::ResetStats()
	{
		s_2Ddata.Stats = Statistics();
	}
}

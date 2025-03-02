#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};

layout(location = 0) out VertexOutput v_Out;
layout(location = 3) flat out float v_TexIndex;
layout(location = 4) flat out int v_EntityID; 

void main()
{
	v_Out.Color = a_Color;
	v_Out.TexCoord = a_TexCoord;
	v_Out.TilingFactor = a_TilingFactor;

	v_TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};

layout(location = 0) in VertexOutput Input;
layout(location = 3) flat in float v_TexIndex;
layout(location = 4) flat in int v_EntityID; 

layout(location = 0) uniform sampler2D u_Texture[32];

void main()
{
	vec4 texColor = Input.Color;
	switch(int(v_TexIndex))
	{
		case 0 : texColor *= texture(u_Texture[0], Input.TexCoord * Input.TilingFactor); break;
		case 1 : texColor *= texture(u_Texture[1], Input.TexCoord * Input.TilingFactor); break;
		case 2 : texColor *= texture(u_Texture[2], Input.TexCoord * Input.TilingFactor); break;
		case 3 : texColor *= texture(u_Texture[3], Input.TexCoord * Input.TilingFactor); break;
		case 4 : texColor *= texture(u_Texture[4], Input.TexCoord * Input.TilingFactor); break;
		case 5 : texColor *= texture(u_Texture[5], Input.TexCoord * Input.TilingFactor); break;
		case 6 : texColor *= texture(u_Texture[6], Input.TexCoord * Input.TilingFactor); break;
		case 7 : texColor *= texture(u_Texture[7], Input.TexCoord * Input.TilingFactor); break;
		case 8 : texColor *= texture(u_Texture[8], Input.TexCoord * Input.TilingFactor); break;
		case 9 : texColor *= texture(u_Texture[9], Input.TexCoord * Input.TilingFactor); break;
		case 10 : texColor *= texture(u_Texture[10], Input.TexCoord * Input.TilingFactor); break;
		case 11 : texColor *= texture(u_Texture[11], Input.TexCoord * Input.TilingFactor); break;
		case 12 : texColor *= texture(u_Texture[12], Input.TexCoord * Input.TilingFactor); break;
		case 13 : texColor *= texture(u_Texture[13], Input.TexCoord * Input.TilingFactor); break;
		case 14 : texColor *= texture(u_Texture[14], Input.TexCoord * Input.TilingFactor); break;
		case 15 : texColor *= texture(u_Texture[15], Input.TexCoord * Input.TilingFactor); break;
		case 16 : texColor *= texture(u_Texture[16], Input.TexCoord * Input.TilingFactor); break;
		case 17 : texColor *= texture(u_Texture[17], Input.TexCoord * Input.TilingFactor); break;
		case 18 : texColor *= texture(u_Texture[18], Input.TexCoord * Input.TilingFactor); break;
		case 19 : texColor *= texture(u_Texture[19], Input.TexCoord * Input.TilingFactor); break;
		case 20 : texColor *= texture(u_Texture[20], Input.TexCoord * Input.TilingFactor); break;
		case 21 : texColor *= texture(u_Texture[21], Input.TexCoord * Input.TilingFactor); break;
		case 22 : texColor *= texture(u_Texture[22], Input.TexCoord * Input.TilingFactor); break;
		case 23 : texColor *= texture(u_Texture[23], Input.TexCoord * Input.TilingFactor); break;
		case 24 : texColor *= texture(u_Texture[24], Input.TexCoord * Input.TilingFactor); break;
		case 25 : texColor *= texture(u_Texture[25], Input.TexCoord * Input.TilingFactor); break;
		case 26 : texColor *= texture(u_Texture[26], Input.TexCoord * Input.TilingFactor); break;
		case 27 : texColor *= texture(u_Texture[27], Input.TexCoord * Input.TilingFactor); break;
		case 28 : texColor *= texture(u_Texture[28], Input.TexCoord * Input.TilingFactor); break;
		case 29 : texColor *= texture(u_Texture[29], Input.TexCoord * Input.TilingFactor); break;
		case 30 : texColor *= texture(u_Texture[30], Input.TexCoord * Input.TilingFactor); break;
		case 31 : texColor *= texture(u_Texture[31], Input.TexCoord * Input.TilingFactor); break;
	}

	if(texColor.a == 0.0)
		discard;

	o_Color = texColor;
	o_EntityID = v_EntityID;
}
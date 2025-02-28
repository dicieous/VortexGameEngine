#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) out VertexOutput v_Out;
layout (location = 4) flat out int v_EntityID;

void main()
{
	v_Out.LocalPosition = a_LocalPosition;
	v_Out.Color = a_Color;
	v_Out.Thickness = a_Thickness;
	v_Out.Fade = a_Fade;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) flat in int v_EntityID;

void main()
{
	// Calculate distance and fill circle with white
	float distance = 1.0 - length(Input.LocalPosition.xy);

	// Smooth fade and thickness calculation
	float circleAlpha = smoothstep(0.0, Input.Fade, distance);
	circleAlpha *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);

	if(circleAlpha == 0.0)
		discard;

	// Set output color
	o_Color = vec4(Input.Color.rgb, Input.Color.a * circleAlpha);
	
	o_EntityID = v_EntityID;
}
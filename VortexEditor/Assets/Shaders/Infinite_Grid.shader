#type vertex
#version 450 core

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

uniform vec3 u_CameraWorldPos;

uniform float u_GridSize;

out float o_GridSize;
out vec3 o_WorldPos;
out vec3 o_CameraWorlPos;

const vec3 Pos[4] = vec3[4]
(
	vec3(-1.0, 0.0, -1.0), //bottom left
	vec3(1.0, 0.0, -1.0),  //bottom right
	vec3(1.0, 0.0, 1.0),   //top right
	vec3(-1.0, 0.0, 1.0)   //top left
);

const uint Indices[6] = uint[6]
(
	0, 2, 1,
	2, 0, 3
);

void main()
{
	uint Index = Indices[gl_VertexID];

	vec3 vPos3 = Pos[Index] * u_GridSize;
	vPos3.x += u_CameraWorldPos.x;
	vPos3.z += u_CameraWorldPos.z;

	vec4 vPos = vec4(vPos3, 1.0);
	gl_Position = u_ViewProjection * vPos;

	o_WorldPos = vPos3;
	o_GridSize = u_GridSize;
	o_CameraWorlPos = u_CameraWorldPos;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int o_EntityID;

in vec3 o_WorldPos;
in float o_GridSize;
in vec3 o_CameraWorlPos;

//make these uniform
uniform float u_GridCellSize;
uniform float u_GridMinPixelsBetweenCells;
uniform vec4 u_GridColorThin;
uniform vec4 u_GridColorThick;

float log10(float x)
{
    float f = log(x) / log(10.0);
    return f;
}


float satf(float x)
{
    float f = clamp(x, 0.0, 1.0);
    return f;
}

float max2(vec2 v)
{
    float f = max(v.x, v.y);
    return f;
}

vec2 satv(vec2 x)
{
    vec2 v = clamp(x, vec2(0.0), vec2(1.0));
    return v;
}

void main()
{
	vec2 dvx = vec2(dFdx(o_WorldPos.x), dFdy(o_WorldPos.x));
	vec2 dvy = vec2(dFdx(o_WorldPos.z), dFdy(o_WorldPos.z));

	float lx = length(dvx);
	float ly = length(dvy);

	vec2 dudv = vec2(lx, ly);

	float l = length(dudv);

	float LOD = max(0.0, log10(l * u_GridMinPixelsBetweenCells/u_GridCellSize) +1.0);

	float GridCellSizeLod0 = u_GridCellSize * pow(10.0, floor(LOD));
	float GridCellSizeLod1 = GridCellSizeLod0 * 10.0;
	float GridCellSizeLod2 = GridCellSizeLod1 * 10.0;

	dudv *= 4.0;

	vec2 mod_div_dudv = mod(o_WorldPos.xz, GridCellSizeLod0) / dudv;
	float Lod0a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)));

	mod_div_dudv = mod(o_WorldPos.xz, GridCellSizeLod1) / dudv;
	float Lod1a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)));

	mod_div_dudv = mod(o_WorldPos.xz, GridCellSizeLod2) / dudv;
	float Lod2a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)));

	float LOD_fade = fract(LOD);

	vec4 color;

	if(Lod2a > 0.0)
	{
		color = u_GridColorThick;
		color.a *= Lod2a;
	}
	else
	{
		if(Lod1a > 0.0)
		{
			color = mix(u_GridColorThick, u_GridColorThin, LOD_fade);
			color.a *= Lod1a;
		}
		else
		{
			color = u_GridColorThin;
			color *= (Lod0a * (1.0 - LOD_fade));
		}
	}

	float OpacityFallOff = (1.5 - satf(length(o_WorldPos.xz - o_CameraWorlPos.xz) / o_GridSize));
	color.a *= OpacityFallOff;

	
    float gridLineThreshold = 0.01;
    if(color.a < gridLineThreshold)
    {
        discard;
    }

	FragColor = color;
	o_EntityID = -1;
}
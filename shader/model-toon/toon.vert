#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 model;
	vec4 lightPos;
} ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outLightVec;
layout (location = 3) out vec2 outCoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	outColor = vec3(0.0, 0.6, 0.2);
	outCoord = inCoord;
	gl_Position = ubo.projection * ubo.model * vec4(inPos.xyz, 1.0);
	outNormal = mat3(ubo.model) * inNormal;
	vec4 pos = ubo.model * vec4(inPos, 1.0);
	vec3 lPos = mat3(ubo.model) * ubo.lightPos.xyz;
	outLightVec = lPos - pos.xyz;
}
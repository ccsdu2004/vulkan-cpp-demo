#version 450
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 model;
	vec4 lightPos;
	float outlineWidth;
} ubo;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	vec4 pos = vec4(inPos.xyz + inNormal * ubo.outlineWidth,1.0);
	gl_Position = ubo.projection * ubo.model * pos;
}

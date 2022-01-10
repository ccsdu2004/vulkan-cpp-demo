#version 450

layout (binding = 1) uniform sampler2D samplerColorMap;
layout (binding = 2) uniform Time 
{
    float current;
} time;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inLightVec;
layout (location = 3) in vec2 inCoord;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 color;
	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightVec);
	float intensity = dot(N,L);
	
	if(intensity > 0.95)
		color = inColor * 1.2;
	else if(intensity > 0.8)
		color = inColor * 1.0;
	else if(intensity > 0.6)
		color = inColor * 0.6;
    else if(intensity > 0.4)
		color = inColor * 0.4;
	else if(intensity > 0.25)
		color = inColor * 0.4;
	else
		color = inColor * 0.1;
		
	outFragColor = mix(texture(samplerColorMap,inCoord),vec4(color,1.0),time.current);
}




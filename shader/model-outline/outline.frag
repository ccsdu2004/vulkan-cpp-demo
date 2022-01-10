#version 450
layout (binding = 1) uniform sampler2D samplerColorMap;
layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = vec4(0.3,0.3,0.6, 1.0); 
}
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout (binding = 0) uniform samplerBuffer texels;
layout (location = 0) out vec4 outColor;
 
void main() {
    float r = texelFetch(texels, 0).r;
    float g = texelFetch(texels, 1).g;
    float b = 0.25;
    outColor = vec4(r, g, b, 1.0);
    gl_Position = vec4(position, 1.0);
}
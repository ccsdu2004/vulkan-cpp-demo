#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout (binding = 0,rgba32f) uniform readonly imageBuffer texels;
layout (location = 0) out vec4 outColor;
 
void main() {
    float r = imageLoad(texels, gl_VertexIndex*3).r;
    float g = imageLoad(texels, gl_VertexIndex*3+1).r;
    float b = imageLoad(texels, gl_VertexIndex*3+2).r;
    outColor = vec4(r,g,b,1.0);
    gl_Position = vec4(position, 1.0);
}
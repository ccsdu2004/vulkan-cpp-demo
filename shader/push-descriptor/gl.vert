#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (std140, binding = 0) uniform buf {
    mat4 mvp;
} ubuf;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 inTexCoords;
layout (location = 0) out vec2 texcoord;
void main() {
   texcoord = inTexCoords;
   gl_Position = ubuf.mvp * vec4(pos,1.0);
}

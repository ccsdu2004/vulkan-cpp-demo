#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
} mvp;

void main() {
    gl_Position = mvp.model * vec4(position, 1.0);
    fragColor = color;
}

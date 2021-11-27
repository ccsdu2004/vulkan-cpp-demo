#version 450
layout(location = 0) in vec4 fragColor;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform UniformColorObject {
    vec4 color;
} color;

void main() {
    outColor = fragColor + color.color;
}

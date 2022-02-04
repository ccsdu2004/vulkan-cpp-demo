#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 coord;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 instancePos;

layout(location = 0) out vec4 fragColor;
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    float value;
} mvp;

void main() {
    gl_Position = mvp.model*vec4(inPosition, 1.0)+vec4(instancePos,1.0);

    if(inPosition.y > mvp.value)
    {
        fragColor = vec4(1.0,0.3,0.3,1.0);
    }
    else
    {
        fragColor = vec4(0.3,0.6,1.0,1.0);
    }
}

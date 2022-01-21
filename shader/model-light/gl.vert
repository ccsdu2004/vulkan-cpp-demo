#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 coord;
layout(location = 2) in vec3 inNormal;
layout(binding = 0) uniform UniformBufferObject {
    mat4 modelView;
    mat4 projection;
} mvp;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPosition;

void main() {
    gl_Position = mvp.projection*mvp.modelView*vec4(inPosition, 1.0);
    fragNormal = mat3(mvp.modelView) * inNormal;
    fragPosition = vec3(mvp.modelView * vec4(inPosition,1.0));
    fragTexCoord = coord;
}

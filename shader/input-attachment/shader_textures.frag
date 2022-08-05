#version 450

//layout(binding = 1) uniform sampler2D texSampler;
layout (input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput inputAttachment;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = texture(texSampler, fragTexCoord);
    outColor = subpassLoad(inputAttachment);
}

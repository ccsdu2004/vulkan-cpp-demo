#version 450
layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPosition;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConsts {
    vec4 position;
    vec4 lightColor;
} pushConsts;

void main() {
    vec3 intensity = vec3(1.0,1.0,1.0);
    vec3 kd = vec3(pushConsts.lightColor);
    vec3 ka = texture(texSampler, fragTexCoord).rgb;
    vec3 ks = vec3(0,1,0);

    float shininess = 64.0;
    vec3 n = normalize(fragNormal);
    vec3 s = normalize(vec3(pushConsts.position) - fragPosition);
    vec3 v = normalize(vec3(-fragPosition));
    vec3 r = reflect(-s,n);

    outColor = vec4(intensity * (ka + kd * max(dot(s,n),0.0) +
           ks * pow(max(dot(r,v),0.5),shininess)),1.0);
}

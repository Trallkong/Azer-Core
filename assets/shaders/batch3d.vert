#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aColor;

layout(set = 1, binding = 0) uniform UniformBufferObject {
    mat4 viewProjection;
    mat4 transform;
} ubo;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec4 vColor;
layout(location = 2) out vec3 vNormal;
layout(location = 3) out vec3 vWorldPos;

void main() {
    vTexCoord = aTexCoord;
    vColor = aColor;
    vec4 worldPos = ubo.transform * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = normalize(mat3(ubo.transform) * aNormal);
    gl_Position = ubo.viewProjection * worldPos;
}

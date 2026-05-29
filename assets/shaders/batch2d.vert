#version 450

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;

layout(set=1, binding = 0) uniform UniformBufferObject {
    mat4 uMVP;
    mat4 _unused;
    float alpha;
} ubo;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec4 vColor;

void main() {
    vTexCoord = aTexCoord;
    vColor = vec4(aColor.rgb, aColor.a * ubo.alpha);
    gl_Position = ubo.uMVP * vec4(aPos, 0.0, 1.0);
}
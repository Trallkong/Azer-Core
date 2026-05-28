#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aColor;

layout(set = 1, binding = 0) uniform UniformBufferObject {
    mat4 viewProjection;
    mat4 transform;
} ubo;

layout(location = 0) out vec3 vDirection;

void main() {
    vDirection = aPos;
    vec4 clipPos = ubo.viewProjection * vec4(aPos, 1.0);
    gl_Position = clipPos.xyww;
}

#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(set = 0, binding = 0) uniform BufferData {
    mat4 viewProjMat;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 modelMat;
    vec4 color;
} pc;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec4 v_Color;

void main() {
    gl_Position = ubo.viewProjMat * pc.modelMat * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_Color = pc.color;
}

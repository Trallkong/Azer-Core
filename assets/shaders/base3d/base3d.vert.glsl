#version 450
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec4 v_Color;
// Vulkan GLSL：非不透明 uniform 必须放进 block，push constant 也必须用 block
layout(set = 0, binding = 0) uniform CameraBlock {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;
layout(set = 0, binding = 1) uniform DrawData {
    mat4 transform;
} drawData;
void main() {
    gl_Position = camera.projectionMatrix * camera.viewMatrix * drawData.transform * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
}

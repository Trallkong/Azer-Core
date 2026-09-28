#version 450
layout(location = 0) in vec3 a_Position;
layout(location = 0) out vec3 v_TexCoords;
layout(set = 0, binding = 0) uniform CameraBlock {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;
void main() {
    v_TexCoords = a_Position;
    vec4 pos = camera.projectionMatrix * camera.viewMatrix * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}

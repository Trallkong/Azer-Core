#version 450

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec4 vColor;

layout(set = 2, binding = 0) uniform sampler2D uTextures;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = texture(uTextures, vTexCoord) * vColor;
}

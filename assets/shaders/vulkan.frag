#version 450

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec4 v_Color;

layout(set = 1, binding = 0) uniform sampler2D u_Texture;

void main()
{
    o_Color = texture(u_Texture, v_TexCoord) * v_Color;
}
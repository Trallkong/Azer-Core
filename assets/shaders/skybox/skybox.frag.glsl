#version 450
layout(location = 0) out vec4 o_Color;
layout(location = 0) in vec3 v_TexCoords;
layout(set = 1, binding = 0) uniform sampler2D u_HdrEquirectangular;
layout(set = 0, binding = 1) uniform PropertyBlock {
    float exposure;
} properties;
vec2 DirToUV(vec3 dir) {
    float theta = acos(max(-1.0, min(1.0, dir.y)));
    float phi = atan(dir.z, dir.x);
    return vec2(phi / (2.0 * 3.1415926) + 0.5, theta / 3.1415926);
}
vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}
void main() {
    vec2 uv = DirToUV(normalize(v_TexCoords));
    vec3 hdrColor = texture(u_HdrEquirectangular, uv).rgb;
    // 色调映射
    vec3 mapped = ACESFilm(hdrColor * properties.exposure);
    // 伽马矫正
    mapped = pow(mapped, vec3(1.0 / 2.2));
    o_Color = vec4(mapped, 1.0);
}

#version 450

layout(location = 0) in vec3 vDirection;

layout(location = 0) out vec4 fragColor;

layout(set = 2, binding = 0) uniform sampler2D uTexture;

const vec2 INV_ATAN = vec2(0.15915494309189535, 0.3183098861837907);

vec2 SampleSphericalMap(vec3 direction) {
    vec3 dir = normalize(direction);
    vec2 uv = vec2(atan(dir.z, dir.x), asin(clamp(dir.y, -1.0, 1.0)));
    uv *= INV_ATAN;
    uv += 0.5;
    return uv;
}

void main() {
    vec3 hdrColor = texture(uTexture, SampleSphericalMap(vDirection)).rgb;
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    mapped = pow(mapped, vec3(1.0 / 2.2));
    fragColor = vec4(mapped, 1.0);
}

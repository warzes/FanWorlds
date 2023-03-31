layout(location = 0) in vec4 vWorldPosition;
layout(location = 1) in vec4 vWorldNormal;
layout(location = 2) in vec2 vTexCoord;

layout(location = 0) out vec4 fWorldPosition;
layout(location = 1) out vec4 fWorldNormal;
layout(location = 2) out vec4 fDiffuse;
layout(location = 3) out vec4 fEmissive;

layout(binding = 0) uniform sampler2D uDiffuseTexture;
layout(binding = 1) uniform sampler2D uEmissiveTexture;

void main() {
    fWorldPosition = vWorldPosition;
    fWorldNormal = normalize(vWorldNormal);
    fDiffuse = texture(uDiffuseTexture, vTexCoord);
    fEmissive = texture(uEmissiveTexture, vTexCoord) * (sin(uTime * PI) * 0.5 + 0.5);
}

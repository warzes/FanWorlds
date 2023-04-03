layout(location = 0) in vec4 vWorldPosition;
layout(location = 1) in vec3 vColor;

layout(location = 0) out vec4 fWorldPosition;
layout(location = 1) out vec4 fWorldNormal;
layout(location = 2) out vec4 fDiffuse;
layout(location = 3) out vec4 fEmissive;

void main() {
    fWorldPosition = vWorldPosition;
    fWorldNormal = vec4(0, 0, 0, 0);
    fDiffuse = vec4(0, 0, 0, 1);
    fEmissive = vec4(vColor, 1);
}

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec4 vWorldPosition;
layout(location = 1) out vec4 vWorldNormal;
layout(location = 2) out vec2 vTexCoord;

layout(location = 0) uniform mat4 uModel;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1);
    vWorldPosition = uModel * vec4(aPosition, 1);
    vWorldNormal = uModel * vec4(aNormal, 0);
    vTexCoord = aTexCoord;
}

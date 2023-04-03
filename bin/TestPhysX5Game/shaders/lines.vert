layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec4 vWorldPosition;
layout(location = 1) out vec3 vColor;

void main() {
    gl_Position = uProjection * uView * vec4(aPosition, 1);
    vWorldPosition = vec4(aPosition, 1);
    vColor = aColor;
}

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

layout(location = 0) out vec4 vNormal;

layout(location = 0) uniform mat4 uModel;

void main() {
    gl_Position = uModel * vec4(aPosition, 1);
    vNormal = uModel * vec4(aNormal, 0);
}

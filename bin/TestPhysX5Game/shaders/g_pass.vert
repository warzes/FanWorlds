layout(location = 0) in vec2 aPosition;

layout(location = 0) out vec2 vTexCoord;

void main() {
    gl_Position = vec4(aPosition, 0, 1);
    vTexCoord = aPosition * 0.5f + 0.5f;
}

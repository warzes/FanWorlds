layout(location = 0) in vec3 gWorldNormal;

void main()
{
    float slopeScale = 1 - abs(dot(normalize(uDirectionalLight), normalize(gWorldNormal)));
    gl_FragDepth = gl_FragCoord.z + 0.008 * slopeScale / gl_FragCoord.w;
}

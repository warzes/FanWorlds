layout(std140, binding = 0) uniform ShaderGlobals {
    mat4 uView;
    mat4 uProjection;
    float uTime;
};

struct PointLightData {
    vec3 position;
    float linear;
    vec3 color;
    float quadratic;
};

layout(std140, binding = 1) uniform LightGlobals {
    vec3 uDirectionalLight;
    float Padding0;
    vec3 uDirectionalColor;
    float Padding1;
    vec3 uAmbientColor;
    float Padding2;
    vec3 uCascadeShadowMapSplits;
    float Padding3;
    mat4 uShadowMatrices[4];
    PointLightData uPointLightData[32];
};

#define PI      3.141592653589793
#define TWO_PI  6.283185307179586
#define INF     (1.0 / 0.0)

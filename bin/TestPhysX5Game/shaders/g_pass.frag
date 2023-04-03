layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 fColor;

layout(binding = 0) uniform sampler2D uWorldPosition;
layout(binding = 1) uniform sampler2D uWorldNormal;
layout(binding = 2) uniform sampler2D uDiffuse;
layout(binding = 3) uniform sampler2D uEmissive;
layout(binding = 4) uniform sampler2DArrayShadow uShadowMap;

float LightDiffuse(vec3 worldNormal, vec3 lightDirection) {
    return max(0, dot(worldNormal, normalize(lightDirection)));
}

float LightDiffuse(vec3 worldNormal, vec3 worldPosition, vec3 lightPosition, float linear, float quadratic)
{
    const vec3 delta = lightPosition - worldPosition;
    const float diffuse = LightDiffuse(worldNormal, delta);
    const float dist = length(delta);
    // point light attenuation from https://wiki.ogre3d.org/-Point+Light+Attenuation
    const float attenuation = fma(fma(quadratic, dist, linear), dist, 1.0);
    return diffuse / attenuation;
}

float BlurShadow(vec3 shadowCoord, int layer) {
    const vec2 texelSize = 1.0 / textureSize(uShadowMap, 0).xy;
    float shadow = 0.0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            vec4 coord = vec4(shadowCoord.xy + vec2(x, y) * texelSize, layer, shadowCoord.z);
            shadow += texture(uShadowMap, coord);
        }
    }
    shadow /= 25.0;
    return shadow;
}

int GetShadowLayer(vec4 viewSpacePos)
{
    const float depth = abs(viewSpacePos.z);
    int layer = 0;
    if (depth > uCascadeShadowMapSplits[0]) layer = 1;
    if (depth > uCascadeShadowMapSplits[1]) layer = 2;
    if (depth > uCascadeShadowMapSplits[2]) layer = 3;
    return layer;
}

float ReadShadowMap(vec4 viewSpacePos, vec4 worldPos, vec3 worldNormal)
{
    int layer = GetShadowLayer(viewSpacePos);
    vec4 lightSpacePos = uShadowMatrices[layer] * worldPos;
    vec3 shadowCoord = lightSpacePos.xyz / lightSpacePos.w;
    shadowCoord = shadowCoord * 0.5 + 0.5;
    return shadowCoord.z > 1.0 ? 0.0 : BlurShadow(shadowCoord, layer);
}

vec3 ACESToneMapping(vec3 color) {
    const vec3 A = vec3(2.51);
    const vec3 B = vec3(0.03);
    const vec3 C = vec3(2.43);
    const vec3 D = vec3(0.59);
    const vec3 E = vec3(0.14);
    return color * fma(color, A, B) / fma(color, fma(color, C, D), E);
}

void main() {
    vec4 worldPosition = texture(uWorldPosition, vTexCoord);
    vec4 viewSpacePosition = uView * worldPosition;
    vec3 worldNormal = normalize(texture(uWorldNormal, vTexCoord).xyz);
    vec3 diffuse = texture(uDiffuse, vTexCoord).rgb;
    vec3 emissive = texture(uEmissive, vTexCoord).rgb;

    vec3 diffuseLighting = vec3(0.0);

    // directional shadow
    float shadow = ReadShadowMap(viewSpacePosition, worldPosition, worldNormal);

    // directional light
    diffuseLighting += mix(uAmbientColor, uDirectionalColor, (1 - shadow) * LightDiffuse(worldNormal, uDirectionalLight));

    // point lights
    for (int i = 0; i < 32; i++) {
        PointLightData pointLightData = uPointLightData[i];
        if (pointLightData.color.r > 0 || pointLightData.color.g > 0 || pointLightData.color.b > 0) {
            float intensity = LightDiffuse(worldNormal, worldPosition.xyz, pointLightData.position, pointLightData.linear, pointLightData.quadratic);
            diffuseLighting += pointLightData.color * intensity;
        }
    }

    // collect all lighting
    vec3 color = fma(diffuse, diffuseLighting, emissive);

    color = ACESToneMapping(color);

    fColor = vec4(color, 1);
}

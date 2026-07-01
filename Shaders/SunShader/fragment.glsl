#version 430 core
layout(location = 0) out vec4 FragColor;

#define MAX_POINT_LIGHTS 64

struct PointLight {
    vec3 position;
    vec3 diffuse;
    vec3 specular;

    // uitdoving
    float constant;
    float lineair;
    float quadratic;
};
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform int uNumPointLights;

#define MAX_DIRECTIONAL_LIGHTS 5

struct DirectionalLight {
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight uDirectionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform int uNumDirectionalLights;

uniform vec3 uAmbient;

struct MaterialProperties {
    int materialType;
    float diffuse;
    float specular;
    float shininess;
};

uniform MaterialProperties uMaterialProperties;

uniform samplerCube uCubemap;
uniform samplerCube uNormalCubemap;
uniform vec3 uColor;

uniform vec3 uCameraPos; // camera location
in vec3 FragPos; // pixel position
in vec3 Normal;
in vec3 LocalPos;

in vec3 Tangent;
in vec3 Bitangent;

const float PI = 3.14159265359;

in float Height;

const int COLOR_SUBDIVISIONS = 10;

uniform int uColorCount;
uniform vec3 uColors[COLOR_SUBDIVISIONS];
uniform float uTopHeight;
uniform float uBottomHeight;

vec3 getNormal() {
    vec3 normalRGB = texture(uNormalCubemap, normalize(LocalPos)).rgb;
    vec3 convertedNormal = (normalRGB * 2.0) - vec3(1.0);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Tangent);
    vec3 B = normalize(Bitangent);

    mat3 tbn = mat3(T, B, N);

    return normalize(tbn * convertedNormal);
}

vec3 getColor() {
    float factor = clamp((Height - uBottomHeight) / (uTopHeight - uBottomHeight), 0.0f, 1.0f);

    float continuousIdx = factor * (uColorCount - 1);

    int lowerIdx = int(floor(continuousIdx));
    int upperIdx = int(ceil(continuousIdx));
    float blend = continuousIdx - float(lowerIdx);

    vec3 finalColor = mix(uColors[lowerIdx], uColors[upperIdx], blend);

    return finalColor;
}

void main()
{
    vec4 texColor = vec4(getColor(), 1.0f);

    vec3 norm = getNormal();
    vec3 viewDir = normalize(FragPos - uCameraPos);

    vec4 outColor = texColor;
    FragColor = outColor;
}

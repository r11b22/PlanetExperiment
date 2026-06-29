#version 330 core
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

const int COLOR_SUBDIVISIONS = 3;
vec3 color[COLOR_SUBDIVISIONS];
float topHeight = 0.60f;
float bottomHeight = 0.30f;

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
    float factor = clamp((Height - bottomHeight) / (topHeight - bottomHeight), 0.0f, 1.0f);

    float continuousIdx = factor * (COLOR_SUBDIVISIONS - 1);

    int lowerIdx = int(floor(continuousIdx));
    int upperIdx = int(ceil(continuousIdx));
    float blend = continuousIdx - float(lowerIdx);

    vec3 finalColor = mix(color[lowerIdx], color[upperIdx], blend);

    return finalColor;
}

void main()
{
    color[0] = vec3(1.0f, 1.0f, 1.0f);
    color[1] = vec3(0.0f, 1.0f, 1.0f);
    color[2] = vec3(1.0f, 0.0f, 1.0f);

    vec4 texColor = vec4(getColor(), 1.0f);

    vec3 norm = getNormal();
    vec3 viewDir = normalize(FragPos - uCameraPos);
    vec3 totalLight = uAmbient * vec3(texColor);

    for (int i = 0; i < uNumPointLights; i++) {
        PointLight light = uPointLights[i];

        vec3 lightDirection = normalize(light.position - FragPos);
        float distance = length(light.position - FragPos);

        float diff = max(dot(norm, lightDirection), 0.0);

        vec3 reflectDir = reflect(-lightDirection, norm);
        float spec = pow(max(dot(-viewDir, reflectDir), 0.0), uMaterialProperties.shininess);

        float attenuation = 1.0 / (light.constant + light.lineair * distance + light.quadratic * (distance * distance));
        vec3 diffuse = uMaterialProperties.diffuse * light.diffuse * diff * vec3(texColor);
        vec3 specular = uMaterialProperties.specular * light.specular * spec;

        totalLight += ((diffuse + specular) * attenuation);
    }

    for (int i = 0; i < uNumDirectionalLights; i++) {
        DirectionalLight light = uDirectionalLights[i];

        vec3 lightDirection = normalize(-light.direction);

        float diff = max(dot(norm, lightDirection), 0.0);

        vec3 reflectDir = reflect(-lightDirection, norm);
        float spec = pow(max(dot(-viewDir, reflectDir), 0.0), uMaterialProperties.shininess);

        vec3 diffuse = uMaterialProperties.diffuse * light.diffuse * diff * vec3(texColor);
        vec3 specular = uMaterialProperties.specular * light.specular * spec;

        totalLight += (diffuse + specular);
    }

    vec4 outColor = vec4(totalLight, texColor.a);
    FragColor = outColor;
}

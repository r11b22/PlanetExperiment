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

const float PI = 3.14159265359;

/*vec4 getTextureColor() {
    // spherical coordinates
    /*float phi = atan(LocalPos.z, LocalPos.x);
    float theta = asin(LocalPos.y);

    float u = phi / (2.0 * PI) + 0.5;
    float v = theta / PI + 0.5;

    vec2 uv = vec2(u, v);

    vec2 dX = dFdx(uv);
    vec2 dY = dFdy(uv);

    if (dX.x > 0.5) dX.x -= 1.0;
    if (dX.x < -0.5) dX.x += 1.0;
    if (dY.x > 0.5) dY.x -= 1.0;
    if (dY.x < -0.5) dY.x += 1.0;*

    return textureGrad(uTexture, uv, dX, dY);
}*/

vec3 getNormal() {
    vec3 normalRGB = texture(uNormalCubemap, normalize(LocalPos)).rgb;
    vec3 convertedNormal = (normalRGB * 2.0) - vec3(1.0);

    vec3 worldNormal = normalize(Normal);

    vec3 N = normalize(worldNormal);

    vec3 up = abs(N.y) > 0.999 ? vec3(0, 0, 1) : vec3(0, 1, 0);

    vec3 T = normalize(cross(up, N));

    vec3 B = cross(N, T);

    mat3 tbn = mat3(T, B, N);

    return normalize(tbn * convertedNormal);
}

void main()
{
    vec4 texColor = texture(uCubemap, normalize(LocalPos));

    if (texColor.a < 0.5) {
        discard;
    }

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

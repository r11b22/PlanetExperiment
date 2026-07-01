#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform samplerCube uDeptCubemap;
uniform float uDepthMultiplier;

out vec3 FragPos;
out vec3 Normal;
out vec3 LocalPos;

out vec3 Tangent;
out vec3 Bitangent;

out float Height;

void calculateTangentAndBitTangent(mat3 normalMatrix) {
    vec3 localNormal = normalize(aPos);

    vec3 localTangent;
    if (abs(localNormal.y) > 0.999) {
        localTangent = vec3(1.0, 0.0, 0.0);
    } else {
        localTangent = normalize(cross(vec3(0.0, 1.0, 0.0), localNormal));
    }

    vec3 localBitangent = cross(localNormal, localTangent);

    Tangent = normalize(normalMatrix * localTangent);
    Bitangent = normalize(normalMatrix * localBitangent);
}

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(uModelMatrix)));

    Normal = normalMatrix * aNorm;
    LocalPos = aPos;

    calculateTangentAndBitTangent(normalMatrix);

    float depthOffset = texture(uDeptCubemap, aPos).r;
    Height = depthOffset;
    depthOffset *= uDepthMultiplier;

    vec3 vertexPos = aPos + (aNorm * depthOffset);

    FragPos = vec3(uModelMatrix * vec4(vertexPos, 1.0));
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(FragPos, 1.0);
}

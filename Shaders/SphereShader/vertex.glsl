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

void main()
{
    Normal = mat3(transpose(inverse(uModelMatrix))) * aNorm;

    LocalPos = aPos;

    float depthOffset = texture(uDeptCubemap, aPos).r;
    depthOffset *= uDepthMultiplier;

    vec3 vertexPos = aPos + (aNorm * depthOffset);

    FragPos = vec3(uModelMatrix * vec4(vertexPos, 1.0));
    // vec4 position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * position;
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(FragPos, 1.0);
}

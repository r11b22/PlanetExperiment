#version 430 core

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

void main() {
    TexCoords = aPos;
    mat4 rotationlessViewMatrix = mat4(mat3(uViewMatrix));

    vec4 pos = uProjectionMatrix * rotationlessViewMatrix * vec4(aPos, 1.0);

    gl_Position = pos.xyww; // put the point at infinity
}

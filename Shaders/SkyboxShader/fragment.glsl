#version 430 core

in vec3 TexCoords;
out vec4 FragColor;
uniform samplerCube uTexture;

void main() {
    FragColor = texture(uTexture, TexCoords);
}

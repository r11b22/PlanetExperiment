#version 430 core

#ifdef GL_ES
precision mediump float;
#endif

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(binding = 0) uniform samplerCube depthmap;

layout(rgba8, binding = 1) writeonly uniform image2D outputTexture;

const int FACE_RIGHT = 0;
const int FACE_LEFT = 1;
const int FACE_TOP = 2;
const int FACE_BOTTOM = 3;
const int FACE_FRONT = 4;
const int FACE_BACK = 5;

uniform int uSide;

vec3 uvToCubeDir(int face, vec2 uv, int size) {
    vec2 st = (uv + 0.5) / vec2(size, size) * 2.0 - 1.0;

    switch (face) {
        case 0:
        return vec3(1.0, -st.y, -st.x); // FACE_RIGHT
        case 1:
        return vec3(-1.0, -st.y, st.x); // FACE_LEFT
        case 2:
        return vec3(st.x, 1.0, st.y); // FACE_TOP
        case 3:
        return vec3(st.x, -1.0, -st.y); // FACE_BOTTOM
        case 4:
        return vec3(st.x, -st.y, 1.0); // FACE_FRONT
        case 5:
        return vec3(-st.x, -st.y, -1.0); // FACE_BACK
    }
    return vec3(0.0);
}

float getHeightAt(int face, int u, int v, int size) {
    vec3 dir = uvToCubeDir(face, vec2(u, v), size);
    return textureLod(depthmap, dir, 0.0).r;
}

vec3 calculateNormalPixel(int face, int u, int v, int size) {
    const mat3 kernelU = mat3(
            vec3(-1.0, 0.0, 1.0), // Column 1
            vec3(-2.0, 0.0, 2.0), // Column 2
            vec3(-1.0, 0.0, 1.0) // Column 3
        );
    const mat3 kernelV = mat3(
            vec3(-1.0, -2.0, -1.0), // Column 1
            vec3(0.0, 0.0, 0.0), // Column 2
            vec3(1.0, 2.0, 1.0) // Column 3
        );

    float du = 0.0;
    float dv = 0.0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            float height = getHeightAt(face, u + j, v + i, size);

            int kernelY = i + 1;
            int kernelX = j + 1;

            du += height * kernelU[kernelX][kernelY];
            dv += height * kernelV[kernelX][kernelY];
        }
    }

    float bumpStrength = 2.0;

    vec3 bumpedVector = vec3(-du * bumpStrength, -dv * bumpStrength, 1.0);

    vec3 localNormal = normalize(bumpedVector);

    vec3 packedColor = (localNormal * 0.5) + vec3(0.5);

    packedColor = clamp(packedColor, 0.0, 1.0);

    return packedColor;
}

void generateSide(int face, int x, int y, int size) {
    vec3 color = calculateNormalPixel(face, x, y, size);

    imageStore(outputTexture, ivec2(x, y), vec4(color, 1.0));
}

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    ivec2 cubeSize = imageSize(outputTexture);
    if (pixelCoord.x >= cubeSize.x || pixelCoord.y >= cubeSize.y) {
        return;
    }
    generateSide(uSide, pixelCoord.x, pixelCoord.y, cubeSize.x);
}

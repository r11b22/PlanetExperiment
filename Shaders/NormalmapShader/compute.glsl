#version 430 core

#ifdef GL_ES
precision mediump float;
#endif

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(r8, binding = 0) readonly uniform imageCube depthmap;

layout(rgba8, binding = 1) writeonly uniform imageCube outputTexture;

const int FACE_RIGHT = 0;
const int FACE_LEFT = 1;
const int FACE_TOP = 2;
const int FACE_BOTTOM = 3;
const int FACE_FRONT = 4;
const int FACE_BACK = 5;

float getRaw(int face, int u, int v) {
    ivec2 cubeSize = imageSize(depthmap);
    if (u >= cubeSize.x || v >= cubeSize.y || face >= 6) {
        return 0.0;
    }

    return imageLoad(depthmap, ivec3(u, v, face)).r;
}

float getHeightAt(int face, int u, int v, int size) {
    int targetU = u;
    int targetV = v;
    int targetFace = face;

    if (targetU < 0) {
        targetU = size + u - 1;

        // underflow
        switch (targetFace) {
            case FACE_FRONT:
            targetFace = FACE_LEFT;
            break;
            case FACE_BACK:
            targetFace = FACE_RIGHT;
            break;
            case FACE_LEFT:
            targetFace = FACE_BACK;
            break;
            case FACE_RIGHT:
            targetFace = FACE_FRONT;
            break;
            case FACE_TOP:
            targetFace = FACE_LEFT;
            targetV = -u;
            targetU = v;
            break;
            case FACE_BOTTOM:
            targetFace = FACE_LEFT;
            targetV = size + u - 1;
            targetU = size - v - 1;
            break;
        }
    } else if (targetU >= size) {
        // overflow
        targetU = u - size;
        switch (targetFace) {
            case FACE_FRONT:
            targetFace = FACE_RIGHT;
            break;
            case FACE_BACK:
            targetFace = FACE_LEFT;
            break;
            case FACE_LEFT:
            targetFace = FACE_FRONT;
            break;
            case FACE_RIGHT:
            targetFace = FACE_BACK;
            break;
            case FACE_TOP:
            targetFace = FACE_RIGHT;

            targetV = u - size;
            targetU = size - v - 1;
            break;
            case FACE_BOTTOM:
            targetFace = FACE_RIGHT;

            targetV = size - (u - size) - 1;
            targetU = v;
            break;
        }
    }

    if (targetV < 0) {
        // underflow

        switch (targetFace) {
            case FACE_FRONT:
            targetFace = FACE_TOP;
            targetV = size + v - 1;
            break;
            case FACE_BACK:
            targetFace = FACE_TOP;
            targetV = -v;
            targetU = size - u - 1;
            break;
            case FACE_LEFT:
            targetFace = FACE_TOP;
            targetU = -v;
            targetV = u;
            break;
            case FACE_RIGHT:
            targetFace = FACE_TOP;
            targetU = size + v - 1;
            targetV = size - u - 1;
            break;
            case FACE_TOP:
            targetFace = FACE_BACK;
            targetV = -v;
            targetU = size - u - 1;
            break;
            case FACE_BOTTOM:
            targetFace = FACE_FRONT;
            targetV = size + v - 1;
            break;
        }
    } else if (targetV >= size) {
        // overflow
        switch (targetFace) {
            case FACE_FRONT:
            targetFace = FACE_BOTTOM;
            targetV = v - size;
            break;
            case FACE_BACK:
            targetFace = FACE_BOTTOM;
            targetV = size - (v - size) - 1;
            targetU = size - u - 1;
            break;
            case FACE_LEFT:
            targetFace = FACE_BOTTOM;
            targetV = size - u - 1;
            targetU = v - size;
            break;
            case FACE_RIGHT:
            targetFace = FACE_BOTTOM;
            targetV = u;
            targetU = size - (v - size) - 1;
            break;
            case FACE_TOP:
            targetFace = FACE_FRONT;
            targetV = v - size;
            break;
            case FACE_BOTTOM:
            targetFace = FACE_BACK;
            targetV = size - (v - size) - 1;
            targetU = size - u - 1;
            break;
        }
    }

    return getRaw(targetFace, targetU, targetV);
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

            // normalize the heigth
            height /= 255;

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

    // default value for now
    ivec3 cubeCoord = ivec3(x, y, face);
    imageStore(outputTexture, cubeCoord, vec4(color, 1.0));
}

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    int faceIndex = int(gl_GlobalInvocationID.z);

    ivec2 cubeSize = imageSize(outputTexture);
    if (pixelCoord.x >= cubeSize.x || pixelCoord.y >= cubeSize.y) {
        return;
    }
    generateSide(faceIndex, pixelCoord.x, pixelCoord.y, cubeSize.x);
}

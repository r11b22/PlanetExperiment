#version 430 core

#ifdef GL_ES
precision mediump float;
#endif

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(r8, binding = 0) writeonly uniform imageCube outputTexture;

uniform vec3 uSeed;
uniform float uNoiseScale;

uniform int uOctaves;
uniform float uLacunarity;
uniform float uGain;

const int FACE_RIGHT = 0;
const int FACE_LEFT = 1;
const int FACE_TOP = 2;
const int FACE_BOTTOM = 3;
const int FACE_FRONT = 4;
const int FACE_BACK = 5;

// Fractal Noise

float hash(vec3 p) {
    p = fract(p * vec3(123.34, 456.21, 789.12));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y * p.z);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);

    vec3 u = f * f * (3.0 - 2.0 * f);

    float frontBottom = mix(hash(i + vec3(0.0, 0.0, 0.0)), hash(i + vec3(1.0, 0.0, 0.0)), u.x);
    float frontTop = mix(hash(i + vec3(0.0, 1.0, 0.0)), hash(i + vec3(1.0, 1.0, 0.0)), u.x);
    float frontFace = mix(frontBottom, frontTop, u.y);

    float backBottom = mix(hash(i + vec3(0.0, 0.0, 1.0)), hash(i + vec3(1.0, 0.0, 1.0)), u.x);
    float backTop = mix(hash(i + vec3(0.0, 1.0, 1.0)), hash(i + vec3(1.0, 1.0, 1.0)), u.x);
    float backFace = mix(backBottom, backTop, u.y);

    return mix(frontFace, backFace, u.z);
}

float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;

    for (int i = 0; i < uOctaves; i++) {
        value += amplitude * noise(p * frequency);
        frequency *= uLacunarity;
        amplitude *= uGain;
    }
    return value;
}

// depthmap

float normalizePixelCoordinates(int pix, float size) {
    return ((pix / size) * 2.0) - 1.0;
}

vec3 uvToVector(int face, float u, float v) {
    vec3 dir = vec3(0.0);

    switch (face) {
        case FACE_RIGHT:
        dir = vec3(1.0, -v, -u);
        break;
        case FACE_LEFT:
        dir = vec3(-1.0, -v, u);
        break;
        case FACE_TOP: // +Y
        dir = vec3(u, 1.0, v);
        break;
        case FACE_BOTTOM: // -Y
        dir = vec3(u, -1.0, -v);
        break;
        case FACE_FRONT: // +Z
        dir = vec3(u, -v, 1.0);
        break;
        case FACE_BACK: // -Z
        dir = vec3(-u, -v, -1.0);
        break;
    }

    return normalize(dir);
}

void generateSide(int face, int x, int y, int size) {
    float u = normalizePixelCoordinates(x, size);
    float v = normalizePixelCoordinates(y, size);

    vec3 vector = uvToVector(face, u, v);
    vector += uSeed;

    float randomValue = noise(vector * uNoiseScale);

    // default value for now
    ivec3 cubeCoord = ivec3(x, y, face);
    imageStore(outputTexture, cubeCoord, vec4(randomValue, 0.0, 0.0, 0.0));
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

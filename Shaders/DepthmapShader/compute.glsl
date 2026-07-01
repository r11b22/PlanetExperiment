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

vec3 hashGradient(vec3 p) {
    p = vec3(
            dot(p, vec3(127.1, 311.7, 74.7)),
            dot(p, vec3(269.5, 183.3, 246.1)),
            dot(p, vec3(113.5, 271.9, 124.6))
        );
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);

    vec3 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

    float n000 = dot(hashGradient(i + vec3(0.0, 0.0, 0.0)), f - vec3(0.0, 0.0, 0.0));
    float n100 = dot(hashGradient(i + vec3(1.0, 0.0, 0.0)), f - vec3(1.0, 0.0, 0.0));
    float n010 = dot(hashGradient(i + vec3(0.0, 1.0, 0.0)), f - vec3(0.0, 1.0, 0.0));
    float n110 = dot(hashGradient(i + vec3(1.0, 1.0, 0.0)), f - vec3(1.0, 1.0, 0.0));
    float n001 = dot(hashGradient(i + vec3(0.0, 0.0, 1.0)), f - vec3(0.0, 0.0, 1.0));
    float n101 = dot(hashGradient(i + vec3(1.0, 0.0, 1.0)), f - vec3(1.0, 0.0, 1.0));
    float n011 = dot(hashGradient(i + vec3(0.0, 1.0, 1.0)), f - vec3(0.0, 1.0, 1.0));
    float n111 = dot(hashGradient(i + vec3(1.0, 1.0, 1.0)), f - vec3(1.0, 1.0, 1.0));

    float nx00 = mix(n000, n100, u.x);
    float nx10 = mix(n010, n110, u.x);
    float nx01 = mix(n001, n101, u.x);
    float nx11 = mix(n011, n111, u.x);

    float nxy0 = mix(nx00, nx10, u.y);
    float nxy1 = mix(nx01, nx11, u.y);

    // put it in [-1.0f, 1.0f]
    return mix(nxy0, nxy1, u.z);
}

float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    float maxAmplitude = 0.0;
    for (int i = 0; i < uOctaves; i++) {
        value += amplitude * noise(p * frequency);
        maxAmplitude += amplitude;
        frequency *= uLacunarity;
        amplitude *= uGain;
    }
    return ((value / maxAmplitude) + 1.0) / 2.0;
}

// depthmap

float normalizePixelCoordinates(int pix, float size) {
    return (((float(pix) + 0.5) / size) * 2.0) - 1.0;
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

    float randomValue = fbm(vector * uNoiseScale);

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

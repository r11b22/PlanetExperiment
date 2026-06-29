#include "Generation/NormalmapGenerator.hpp"
#include "Texture/CubemapTexture.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"

NormalmapGenerator::NormalmapGenerator(int size, const unsigned char* front, const unsigned char* back, const unsigned char* left, const unsigned char* right, const unsigned char* top, const unsigned char* bottom)
    : mSize(size)
{
    mFaces.resize(6);

    mFaces[static_cast<int>(CubeFace::Front)] = front;
    mFaces[static_cast<int>(CubeFace::Back)] = back;
    mFaces[static_cast<int>(CubeFace::Left)] = left;
    mFaces[static_cast<int>(CubeFace::Right)] = right;
    mFaces[static_cast<int>(CubeFace::Top)] = top;
    mFaces[static_cast<int>(CubeFace::Bottom)] = bottom;
}

CubemapTexture NormalmapGenerator::generateDepthmap(std::string name){
    CubemapTexture tex{name, mSize, mSize, CHANNELS};

    // Helper lambda to generate, upload, and instantly free the face data
    auto uploadAndFreeSide = [&](CubeFace face) {
        auto* data = generateSide(face);
        tex.setSidePixelData(face, data);

        delete [] data;
    };

    uploadAndFreeSide(CubeFace::Front);
    uploadAndFreeSide(CubeFace::Back);
    uploadAndFreeSide(CubeFace::Top);
    uploadAndFreeSide(CubeFace::Bottom);
    uploadAndFreeSide(CubeFace::Right);
    uploadAndFreeSide(CubeFace::Left);

    return std::move(tex);
}

const unsigned char* NormalmapGenerator::generateSide(CubeFace face){
    unsigned char *data =  new unsigned char[mSize * mSize * CHANNELS];

    for (int i = 0; i < mSize; i++){
        for (int j = 0; j < mSize; j++){
            glm::vec3 color = calculateNormalPixel(face, j, i);

            int pixelIndex = (i * mSize + j) * 3;
            data[pixelIndex + 0] = static_cast<unsigned char>(color.r);
            data[pixelIndex + 1] = static_cast<unsigned char>(color.g);
            data[pixelIndex + 2] = static_cast<unsigned char>(color.b);
        }
    }

    return data;
}

glm::vec3 NormalmapGenerator::calculateNormalPixel(CubeFace face, int u, int v){
    const float kernelU[3][3] = {
        { -1.0f,  0.0f,  1.0f },
        { -2.0f,  0.0f,  2.0f },
        { -1.0f,  0.0f,  1.0f }
    };

    const float kernelV[3][3] = {
        { -1.0f, -2.0f, -1.0f },
        {  0.0f,  0.0f,  0.0f },
        {  1.0f,  2.0f,  1.0f }
    };

    float du = 0.0f;
    float dv = 0.0f;

    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            float height = getHeightAt(face, u+j, v+i);

            int kernelY = i + 1;
            int kernelX = j + 1;

            du += height * kernelU[kernelY][kernelX];
            dv += height * kernelV[kernelY][kernelX];
        }
    }

    float bumpStrength = 2.0f;
    glm::vec3 bumpedVector = glm::vec3(-du * bumpStrength, -dv * bumpStrength, 1.0f);

    glm::vec3 localNormal = glm::normalize(bumpedVector);

    glm::vec3 packedColor = (localNormal * 0.5f) + 0.5f;

    packedColor *= 255.0f;

    packedColor = glm::clamp(packedColor, 0.0f, 255.0f);

    return packedColor;
}

float NormalmapGenerator::getHeightAt(CubeFace face, int u, int v) {
    int targetU = u;
    int targetV = v;
    CubeFace targetFace = face;

    if (targetU < 0){
        targetU = mSize+u;

        // underflow
        switch (targetFace) {
            case CubeFace::Front:
                targetFace = CubeFace::Left;
            break;
            case CubeFace::Back:
                targetFace = CubeFace::Right;
            break;
            case CubeFace::Left:
                targetFace = CubeFace::Back;
            break;
            case CubeFace::Right:
                targetFace = CubeFace::Front;
            break;
            case CubeFace::Top:
                targetFace = CubeFace::Left;
                targetV = -u;
                targetU = v;
            break;
            case CubeFace::Bottom:
                targetFace = CubeFace::Left;
                targetV = mSize+u;
                targetU = mSize-v;
            break;
        }
    }else if(targetU >= mSize){
        // overflow
        targetU = u-mSize;
        switch (targetFace) {
            case CubeFace::Front:
                targetFace = CubeFace::Right;
            break;
            case CubeFace::Back:
                targetFace = CubeFace::Left;
            break;
            case CubeFace::Left:
                targetFace = CubeFace::Front;
            break;
            case CubeFace::Right:
                targetFace = CubeFace::Back;
            break;
            case CubeFace::Top:
                targetFace = CubeFace::Right;

                targetV = u-mSize;
                targetU = mSize-v;
            break;
            case CubeFace::Bottom:
                targetFace = CubeFace::Right;

                targetV = mSize - (u - mSize);
                targetU = v;
            break;
        }
    }

    if (targetV < 0){
        // underflow

        switch (targetFace) {
            case CubeFace::Front:
                targetFace = CubeFace::Top;
                targetV = mSize + v;
            break;
            case CubeFace::Back:
                targetFace = CubeFace::Top;
                targetV = -v;
                targetU = mSize-u;
            break;
            case CubeFace::Left:
                targetFace = CubeFace::Top;
                targetU = -v;
                targetV = u;
            break;
            case CubeFace::Right:
                targetFace = CubeFace::Top;
                targetU = mSize + v;
                targetV = mSize - u;
            break;
            case CubeFace::Top:
                targetFace = CubeFace::Back;
                targetV = mSize + v;
                targetU = mSize - u;
            break;
            case CubeFace::Bottom:
                targetFace = CubeFace::Front;
                targetV = mSize + v;
            break;
        }
    }else if (targetV >= mSize){
        // overflow
        switch (targetFace) {
            case CubeFace::Front:
                targetFace = CubeFace::Bottom;
                targetV = v - mSize;
            break;
            case CubeFace::Back:
                targetFace = CubeFace::Bottom;
                targetV = mSize - (v-mSize);
                targetU = mSize - u;
            break;
            case CubeFace::Left:
                targetFace = CubeFace::Bottom;
                targetV = mSize - u;
                targetU = v - mSize;
            break;
            case CubeFace::Right:
                targetFace = CubeFace::Bottom;
                targetV = u;
                targetU = mSize - (v-mSize);
            break;
            case CubeFace::Top:
                targetFace = CubeFace::Front;
                targetV = v - mSize;
            break;
            case CubeFace::Bottom:
                targetFace = CubeFace::Back;
                targetV = mSize - (v-mSize);
                targetU = mSize - u;
            break;
        }
    }


    return mFaces[static_cast<int>(targetFace)][targetV * mSize + targetU];
}

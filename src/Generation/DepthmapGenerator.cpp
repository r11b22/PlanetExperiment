#include "Generation/DepthmapGenerator.hpp"
#include "Texture/CubemapTexture.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "Utilities/Random.h"

DepthmapGenerator::DepthmapGenerator()
    : mNoiseGenerator(getRandomInt(0, 1000000000)), mNoiseScale(getRandomFloat(1.0f, 5.0f))
{

}

DepthmapGenerator::~DepthmapGenerator(){
    clearSideData();
}

CubemapTexture DepthmapGenerator::generateCubemap(std::string name, int size){
    clearSideData();

    mSides.resize(6);

    CubemapTexture tex{name, size, size, CHANNELS};

    // Helper lambda to generate, upload, and instantly free the face data
    auto uploadAndFreeSide = [&](CubeFace face) {
        auto* data = generateSide(face, size);
        tex.setSidePixelData(face, data);

        // Free the memory.
        // NOTE: If generateSide uses malloc instead of new[], use: free(data);
        mSides[static_cast<int>(face)] = data;
    };

    uploadAndFreeSide(CubeFace::Front);
    uploadAndFreeSide(CubeFace::Back);
    uploadAndFreeSide(CubeFace::Top);
    uploadAndFreeSide(CubeFace::Bottom);
    uploadAndFreeSide(CubeFace::Right);
    uploadAndFreeSide(CubeFace::Left);

    return std::move(tex);
}

const unsigned char* DepthmapGenerator::getSide(CubeFace face){
    return mSides[static_cast<int>(face)];
}

const unsigned char* DepthmapGenerator::generateSide(CubeFace face, int size){
    // Create a place for the texture to be generated in. this is a 1 channel texture

    unsigned char *data =  new unsigned char[size * size * CHANNELS];

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            int index = (i * size + j) * CHANNELS;

            float u = normalizePixelCoordinates(j, size);
            float v = normalizePixelCoordinates(i, size);

            glm::vec3 vector = uvToVector(face, u, v);

            float randomValue = mNoiseGenerator.noise(vector.x * mNoiseScale, vector.y * mNoiseScale, vector.z * mNoiseScale, 3, 2.0, 0.35);

            float result = ((randomValue+1.0f)/2.0f)*255;



            data[index + 0] = static_cast<unsigned char>(result);  // R
        }
    }

    return std::move(data);
}

float DepthmapGenerator::normalizePixelCoordinates(float pix, int size){
    return ((pix/size)*2.0f)-1.0f;
}

glm::vec3 DepthmapGenerator::uvToVector(CubeFace face, float u, float v) {
    glm::vec3 dir(0.0f);

    switch (face) {
        case CubeFace::Right:
            dir = glm::vec3(1.0f, -v, -u);
            break;
        case CubeFace::Left:
            dir = glm::vec3(-1.0f, -v, u);
            break;
        case CubeFace::Top:       // +Y
            dir = glm::vec3(u, 1.0f, v);
            break;
        case CubeFace::Bottom:    // -Y
            dir = glm::vec3(u, -1.0f, -v);
            break;
        case CubeFace::Front:     // +Z
            dir = glm::vec3(u, -v, 1.0f);
            break;
        case CubeFace::Back:      // -Z
            dir = glm::vec3(-u, -v, -1.0f);
            break;
    }

    return glm::normalize(dir);
}

void DepthmapGenerator::clearSideData(){
    for (const auto& data : mSides){
        if (data != nullptr){
            delete [] data;
        }
    }
}

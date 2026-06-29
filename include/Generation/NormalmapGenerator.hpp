#pragma once

#include "Texture/CubemapTexture.hpp"
#include "glm/ext/vector_float3.hpp"
#include <vector>
class NormalmapGenerator {
    private:
        static constexpr int CHANNELS = 3;
        std::vector<const unsigned char*> mFaces;
        int mSize;
    public:
        NormalmapGenerator(int size, const unsigned char* front, const unsigned char* back, const unsigned char* left, const unsigned char* right, const unsigned char* top, const unsigned char* bottom);

        CubemapTexture generateDepthmap(std::string name);

    private:
        glm::vec3 calculateNormalPixel(CubeFace face, int u, int v);

        const unsigned char* generateSide(CubeFace face);
        float getHeightAt(CubeFace face, int u, int v);

};

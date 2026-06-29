#pragma once

#include "Random/FractalNoise.hpp"
#include "Random/PerlinNoise.hpp"
#include "Texture/CubemapTexture.hpp"
#include "glm/ext/vector_float3.hpp"
#include <vector>



class DepthmapGenerator {
    private:
        static constexpr int CHANNELS = 1;
        FractalNoise mNoiseGenerator;
        std::vector<const unsigned char*> mSides;
    public:
        DepthmapGenerator();
        ~DepthmapGenerator();

        CubemapTexture generateCubemap(std::string name, int size);

        const unsigned char* getSide(CubeFace face);

    private:
        void clearSideData();


        const unsigned char* generateSide(CubeFace face, int size);

        float normalizePixelCoordinates(float pix, int size);

        glm::vec3 uvToVector(CubeFace face, float u, float v);
};

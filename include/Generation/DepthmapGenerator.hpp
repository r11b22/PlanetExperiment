#pragma once

#include "Random/FractalNoise.hpp"
#include "Random/PerlinNoise.hpp"
#include "Texture/CubemapTexture.hpp"
#include "Shader/ComputeShader.h"
#include "glm/ext/vector_float3.hpp"
#include <vector>



class DepthmapGenerator {
    private:
        static constexpr int CHANNELS = 1;
        FractalNoise mNoiseGenerator;
        std::vector<const unsigned char*> mSides;
        float mNoiseScale;

        int mSize;

        ComputeShader mCompute;
        Texture mFrontTexture;


    public:
        DepthmapGenerator(int size);
        ~DepthmapGenerator();


        CubemapTexture generateCubemap(std::string name);

        const unsigned char* getSide(CubeFace face);

    private:
        void clearSideData();


        const unsigned char* generateSide(CubeFace face);

        float normalizePixelCoordinates(float pix);

        glm::vec3 uvToVector(CubeFace face, float u, float v);
};

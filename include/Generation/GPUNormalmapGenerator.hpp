#pragma once

#include "Shader/ComputeShader.h"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
class GPUNormalmapGenerator {
    private:
        int mSize;

        CubemapTexture mOutputTexture;
        CubemapTexture* mDepthmap;
        ComputeShader mCompute;
    public:
        GPUNormalmapGenerator(int size, CubemapTexture* depthmap);

        CubemapTexture generateNormalmap();
    private:
};

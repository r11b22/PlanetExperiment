#pragma once

#include "Asset/AssetReference.hpp"
#include "Shader/ComputeShader.h"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
class GPUDepthmapGenerator {
    private:
        int mSize;

        CubemapTexture mOutputTexutre;
        ComputeShader mCompute;


    public:
        GPUDepthmapGenerator(int size);

        CubemapTexture generateCubemap();
    private:
};

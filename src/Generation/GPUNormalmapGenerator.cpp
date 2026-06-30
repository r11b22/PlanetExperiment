#include "Generation/GPUNormalmapGenerator.hpp"
#include "Shader/ComputeShader.h"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"


GPUNormalmapGenerator::GPUNormalmapGenerator(int size, CubemapTexture* depthmap)
    : mDepthmap(depthmap), mSize(size),
    mCompute(ComputeShader::createFromPath("Shaders/NormalmapShader/compute.glsl", 16, 16, 1)),
    mOutputTexture("depthMap", size, size, 4)
{
    mOutputTexture.initEmpty();
}

CubemapTexture GPUNormalmapGenerator::generateNormalmap() {
    mCompute.use();

    mDepthmap->imageBind(0, GL_READ_ONLY);
    mOutputTexture.imageBind(1, GL_WRITE_ONLY);

    mCompute.rawDispatch(mSize, mSize, 6);

    return std::move(mOutputTexture);
}

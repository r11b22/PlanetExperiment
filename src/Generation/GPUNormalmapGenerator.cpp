#include "Generation/GPUNormalmapGenerator.hpp"
#include "Shader/ComputeShader.h"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"
#include <vector>


GPUNormalmapGenerator::GPUNormalmapGenerator(int size, CubemapTexture* depthmap)
    : mDepthmap(depthmap), mSize(size),
    mCompute(ComputeShader::createFromPath("Shaders/NormalmapShader/compute.glsl", 16, 16, 1)),
    mOutputTexture("depthMap", size, size, 4)
{
    mOutputTexture.initEmpty();
}

CubemapTexture GPUNormalmapGenerator::generateNormalmap() {
    mCompute.use();

    mDepthmap->bind(0);


    const std::vector<CubeFace> faces = {CubeFace::Right, CubeFace::Left, CubeFace::Front,CubeFace::Back,CubeFace::Bottom, CubeFace::Top};
    for (const auto face : faces){
        mOutputTexture.singleSideImageBind(1, face, GL_WRITE_ONLY);

        mCompute.setUniform({"uSide", static_cast<int>(face)});

        mCompute.rawDispatch(mSize, mSize, 1);
        mCompute.waitForFinish();
    }



    return std::move(mOutputTexture);
}

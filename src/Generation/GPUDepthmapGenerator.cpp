#include "Generation/GPUDepthmapGenerator.hpp"
#include "Shader/ComputeShader.h"
#include "Texture/CubemapTexture.hpp"
#include "Utilities/Random.h"
#include <sys/ucontext.h>
#include <utility>


GPUDepthmapGenerator::GPUDepthmapGenerator(int size)
    : mSize(size), mCompute(ComputeShader::createFromPath("Shaders/DepthmapShader/compute.glsl", 16, 16, 1)),
    mOutputTexutre("depthMap", size, size, 1)
{
    mOutputTexutre.initEmpty();
}


CubemapTexture GPUDepthmapGenerator::generateCubemap(){
    mCompute.use();

    mOutputTexutre.imageBind(0, GL_WRITE_ONLY);

    mCompute.setUniform({"uSeed", getRandomVec3(glm::vec3{-1000.0f}, glm::vec3{1000.0f})});
    mCompute.setUniform({"uNoiseScale", getRandomFloat(1.0f, 5.0f)});

    mCompute.setUniform({"uOctaves", 7});
    mCompute.setUniform({"uLacunarity", 2.0f});
    mCompute.setUniform({"uGain", 0.25f});

    mCompute.rawDispatch(mSize, mSize, 6);
    mCompute.waitForFinish();

    return std::move(mOutputTexutre);
}

#include "Generation/PlanetGenerator.hpp"
#include "Asset/AssetManager.hpp"
#include "Generation/GPUDepthmapGenerator.hpp"
#include "Generation/GPUNormalmapGenerator.hpp"
#include "Object/ObjectRepository.h"
#include "PlanetBody.hpp"
#include "Scene/Scene.hpp"
#include "Texture/CubemapTexture.hpp"
#include "glm/ext/vector_float3.hpp"
#include "Utilities/Random.h"
#include <utility>

PlanetGenerator::PlanetGenerator(int size)
    : mSize(size)
{

}

ObjectReference<PlanetBody> PlanetGenerator::generatePlanet(Scene& scene){
    AssetManager& assetManager = scene.getAssetManager();


    GPUDepthmapGenerator depthGen{mSize};

    CubemapTexture depthTexture = depthGen.generateCubemap();

    GPUNormalmapGenerator normalGen{mSize, &depthTexture};

    CubemapTexture normalTexture = normalGen.generateNormalmap();


    CubemapTextureReference depthMap = assetManager.addAsset(std::move(depthTexture));
    CubemapTextureReference normalMap = assetManager.addAsset(std::move(normalTexture));

    Material planetMaterial = Material{"yes", glm::vec3(1.0f)};
    planetMaterial.setSpecular(0.2f);
    planetMaterial.setShininess(32.0f);

    ObjectReference<PlanetBody> planetBody = scene.createObject<PlanetBody>("earth", planetMaterial, normalMap, depthMap);
    planetBody->setDepthMultiplier(0.25f);
    planetBody->setBottomHeight(0.20f);
    planetBody->setTopHeight(0.80f);
    planetBody->setColors({getRandomVec3(glm::vec3{0.0f}, glm::vec3{1.0f}), getRandomVec3(glm::vec3{0.0f}, glm::vec3{1.0f}), getRandomVec3(glm::vec3{0.0f}, glm::vec3{1.0f})});

    return planetBody;
}

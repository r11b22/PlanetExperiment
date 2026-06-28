#include "Generation/PlanetGenerator.hpp"
#include "Asset/AssetManager.hpp"
#include "Asset/AssetReference.hpp"
#include "Generation/DepthmapGenerator.hpp"
#include "Object/ObjectRepository.h"
#include "PlanetBody.hpp"
#include "Scene/Scene.hpp"
#include "Texture/CubemapTexture.hpp"
#include <utility>

PlanetGenerator::PlanetGenerator(int size)
    : mSize(size)
{

}

ObjectReference<PlanetBody> PlanetGenerator::generatePlanet(Scene& scene){
    AssetManager& assetManager = scene.getAssetManager();

    DepthmapGenerator gen{};

    CubemapTexture depthTexture = gen.generateCubemap("depthTexture", mSize);



    // TODO generate the textures
    CubemapTextureReference texture = assetManager.addAsset(std::move(depthTexture));
    CubemapTextureReference normalMap = assetManager.getAssetByName<CubemapTexture>("earthNormalCubemap");
    CubemapTextureReference depthMap = texture;

    Material planetMaterial = Material{"yes", glm::vec3(1.0f)};
    planetMaterial.setSpecular(0.2f);
    planetMaterial.setShininess(32.0f);

    ObjectReference<PlanetBody> planetBody = scene.createObject<PlanetBody>("earth", planetMaterial, texture, normalMap, depthMap);
    planetBody->setDepthMultiplier(0.5f);

    return planetBody;
}

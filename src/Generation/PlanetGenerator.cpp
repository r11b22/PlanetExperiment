#include "Generation/PlanetGenerator.hpp"
#include "Asset/AssetManager.hpp"
#include "Asset/AssetReference.hpp"
#include "Generation/DepthmapGenerator.hpp"
#include "Generation/NormalmapGenerator.hpp"
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

    NormalmapGenerator normalGen{mSize, gen.getSide(CubeFace::Front), gen.getSide(CubeFace::Back), gen.getSide(CubeFace::Left), gen.getSide(CubeFace::Right), gen.getSide(CubeFace::Top), gen.getSide(CubeFace::Bottom)};
    CubemapTexture normalTexture = normalGen.generateDepthmap("normalTexture");


    assetManager.addAsset(std::move(normalTexture));
    assetManager.addAsset(std::move(depthTexture));

    CubemapTextureReference normalMap = assetManager.getAssetByName<CubemapTexture>("normalTexture");
    CubemapTextureReference depthMap = assetManager.getAssetByName<CubemapTexture>("depthTexture");

    Material planetMaterial = Material{"yes", glm::vec3(1.0f)};
    planetMaterial.setSpecular(0.2f);
    planetMaterial.setShininess(32.0f);

    ObjectReference<PlanetBody> planetBody = scene.createObject<PlanetBody>("earth", planetMaterial, normalMap, depthMap);
    planetBody->setDepthMultiplier(0.5f);

    return planetBody;
}

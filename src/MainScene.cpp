#include "MainScene.hpp"
#include "Asset/AssetLoader.hpp"
#include "Buffer/Buffer.h"
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Defaults/Objects/Lighting/AmbientLight.h"
#include "Defaults/Objects/Lighting/DirectionalLight.h"
#include "FileReader.h"
#include "InputManager.h"
#include "IcoSphere.hpp"
#include "Mesh/MeshReference.hpp"
#include "Object/ObjectRepository.h"
#include "PlanetBody.hpp"
#include "Renderer/Renderer.h"

#include "Defaults/Camera/FirstPersonCamera.h"
#include "Shader/ShaderProgram.h"
#include "Texture/CubemapAsset.hpp"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"
#include "glm/ext/vector_float3.hpp"
#include "Texture/TextureLoader.h"
#include "Texture/CubemapLoader.hpp"


#include <memory>


MainScene::MainScene(){

}

void MainScene::onLoad(Renderer& renderer, Window& window) {
    getAssetManager().loadAsset(AssetLoadInfo<Texture>{"testTexture", "Textures/test_texture.png"});
    getAssetManager().loadAsset(AssetLoadInfo<Texture>{"earthTexture", "Textures/2k_earth_daymap.jpg", true});

    AssetLoadInfo<CubemapTexture> earthMap = {"earthCubemap"};

    // Base directory
    std::filesystem::path baseDir = "Textures/earth/Color";

    // Mapping the faces to their respective file names
    earthMap.setPath(CubeFace::Right,  baseDir / "px.png"); // Positive X
    earthMap.setPath(CubeFace::Left,   baseDir / "nx.png"); // Negative X

    earthMap.setPath(CubeFace::Top,    baseDir / "py.png"); // Positive Y
    earthMap.setPath(CubeFace::Bottom, baseDir / "ny.png"); // Negative Y

    earthMap.setPath(CubeFace::Front,  baseDir / "nz.png"); // Positive Z
    earthMap.setPath(CubeFace::Back,   baseDir / "pz.png"); // Negative Z

    getAssetManager().loadAsset(earthMap);


    AssetLoadInfo<CubemapTexture> earthNormalMap = {"earthNormalCubemap"};

    // Base directory
    std::filesystem::path normalBaseDir = "Textures/earth/Normals";

    // Mapping the faces to their respective file names
    earthNormalMap.setPath(CubeFace::Right,  normalBaseDir / "px.png"); // Positive X
    earthNormalMap.setPath(CubeFace::Left,   normalBaseDir / "nx.png"); // Negative X

    earthNormalMap.setPath(CubeFace::Top,    normalBaseDir / "py.png"); // Positive Y
    earthNormalMap.setPath(CubeFace::Bottom, normalBaseDir / "ny.png"); // Negative Y

    earthNormalMap.setPath(CubeFace::Front,  normalBaseDir / "nz.png"); // Positive Z
    earthNormalMap.setPath(CubeFace::Back,   normalBaseDir / "pz.png"); // Negative Z

    getAssetManager().loadAsset(earthNormalMap);



    window.setVSYNC(false);

    setupSphereShader(renderer);

    InputManager* inputManager = new InputManager{window};

    ObjectReference<FirstPersonCamera> cam = createObject<FirstPersonCamera>("camera", inputManager, &window);


    ObjectReference<AmbientLight> ambientLight = createObject<AmbientLight>("light");
    ObjectReference<DirectionalLight> directionalLight = createObject<DirectionalLight>("directional light", glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{1.0f});



    CubemapTextureReference texture = getAssetManager().getAssetByName<CubemapTexture>("earthCubemap");
    CubemapTextureReference normalMap = getAssetManager().getAssetByName<CubemapTexture>("earthNormalCubemap");

    ObjectReference<PlanetBody> planetBody = createObject<PlanetBody>("earth", Material{"yes", glm::vec3(1.0f)}, texture, normalMap);

    cam->setPosition(glm::vec3{0.0f, 1.0f, 2.0f});
}

void MainScene::setupSphereShader(Renderer& renderer){
    std::unique_ptr<ShaderProgram> sphereProgram = std::make_unique<ShaderProgram>();
    sphereProgram->addShader(FileReader::readFile("Shaders/SphereShader/vertex.glsl").c_str(), GL_VERTEX_SHADER);
    sphereProgram->addShader(FileReader::readFile("Shaders/SphereShader/fragment.glsl").c_str(), GL_FRAGMENT_SHADER);
    sphereProgram->link();
    renderer.addShaderProgram("sphereShader", std::move(sphereProgram));
}

void MainScene::onUpdate(Renderer& renderer, Window& window, float deltaT) {
    ObjectReference<PlanetBody> earth = getObjectByName<PlanetBody>("earth");

    earth->rotate(deltaT*0.5, glm::vec3(0.0f, 1.0f, 0.0f));
}

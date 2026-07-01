#include "MainScene.hpp"
#include "Asset/AssetLoader.hpp"
#include "Asset/AssetReference.hpp"
#include "Buffer/Buffer.h"
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Defaults/Objects/Lighting/AmbientLight.h"
#include "Defaults/Objects/Lighting/DirectionalLight.h"
#include "Defaults/Objects/TransformableObject.h"
#include "FileReader.h"
#include "Generation/PlanetGenerator.hpp"
#include "InputManager.h"
#include "IcoSphere.hpp"
#include "Mesh/MeshReference.hpp"
#include "Object/ObjectRepository.h"
#include "PlanetAnchor.hpp"
#include "PlanetBody.hpp"
#include "Renderer/Renderer.h"

#include "Defaults/Camera/FirstPersonCamera.h"
#include "Shader/ShaderProgram.h"
#include "Texture/CubemapAsset.hpp"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"
#include "Utilities/Random.h"
#include "glm/ext/vector_float3.hpp"
#include "Texture/TextureLoader.h"
#include "Texture/CubemapLoader.hpp"


#include <cmath>
#include <filesystem>
#include <memory>
#include <vector>

#include "Defaults/Objects/Lighting/PointLight.h"



MainScene::MainScene(){
    addAsset(AssetLoadInfo<Texture>{"testTexture", "Textures/test_texture.png"});
    addAsset(AssetLoadInfo<Texture>{"earthTexture", "Textures/2k_earth_daymap.jpg", true});

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

    addAsset(earthMap);


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

    addAsset(earthNormalMap);


    AssetLoadInfo<CubemapTexture> earthDepthMap = {"earthDepthCubemap"};
    // Base directory
    std::filesystem::path baseDepthDir = "Textures/earth/depthmap";

    // Mapping the faces to their respective file names
    earthDepthMap.setPath(CubeFace::Right,  baseDepthDir / "px.png"); // Positive X
    earthDepthMap.setPath(CubeFace::Left,   baseDepthDir / "nx.png"); // Negative X

    earthDepthMap.setPath(CubeFace::Top,    baseDepthDir / "py.png"); // Positive Y
    earthDepthMap.setPath(CubeFace::Bottom, baseDepthDir / "ny.png"); // Negative Y

    earthDepthMap.setPath(CubeFace::Front,  baseDepthDir / "nz.png"); // Positive Z
    earthDepthMap.setPath(CubeFace::Back,   baseDepthDir / "pz.png"); // Negative Z

    addAsset(earthDepthMap);

    AssetLoadInfo<CubemapTexture> uvMap = {"uvMap"};

    std::filesystem::path dir = "Textures/test_uv";

    uvMap.setPath(CubeFace::Right,  dir / "right.png"); // Positive X
    uvMap.setPath(CubeFace::Left,   dir / "left.png"); // Negative X

    uvMap.setPath(CubeFace::Top,    dir / "top.png"); // Positive Y
    uvMap.setPath(CubeFace::Bottom, dir / "bottom.png"); // Negative Y

    uvMap.setPath(CubeFace::Front,  dir / "front.png"); // Positive Z
    uvMap.setPath(CubeFace::Back,   dir / "back.png"); // Negative Z

    addAsset(uvMap);
}

void MainScene::onLoad(Renderer& renderer, Window& window) {



    window.setVSYNC(false);
    renderer.setSetting(GL_TEXTURE_CUBE_MAP_SEAMLESS, true);

    setupShaders(renderer);

    InputManager* inputManager = new InputManager{window};

    ObjectReference<FirstPersonCamera> cam = createObject<FirstPersonCamera>("camera", inputManager, &window);


    ObjectReference<AmbientLight> ambientLight = createObject<AmbientLight>("light", glm::vec3{0.2f});
    //ObjectReference<DirectionalLight> directionalLight = createObject<DirectionalLight>("directional light", glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{1.0f});

    PlanetGenerator sunGen{BASE_RESOLUTION};



    ObjectReference<PlanetBody> sun = sunGen.generatePlanet(*this);
    sun->setScale(glm::vec3{SUN_SCALE});
    sun->setShader("sunShader");



    ObjectReference<PointLight> sunLight = createObject<PointLight>("point", glm::vec3{1.0f});
    sunLight->setParent(sun);


    createPlanets(sun);



    cam->setPosition(glm::vec3{0.0f, 0.0f, 40.0f});
}
void MainScene::createPlanets(ObjectReference<PlanetBody> sun){
    for(int i = 0; i < PLANET_COUNT; i++){
        float size = getRandomFloat(0.5f, 4.0f);

        ObjectReference<PlanetAnchor> anchor = createObject<PlanetAnchor>("anchor", BASE_ROTATION_SPEED/size);



        PlanetGenerator gen{BASE_RESOLUTION};
        ObjectReference<PlanetBody> planet = gen.generatePlanet(*this);
        glm::vec3 offset = getRandomVec3(glm::vec3{DIST_VARIATION}, glm::vec3{DIST_VARIATION});
        float radius = SUN_SCALE + SUN_PADDING + pow(size, SIZE_DIST_RELATION);
        float angle = glm::radians(getRandomFloat(0.0f, 360.0f));

        float x = radius * glm::cos(angle);
        float y = radius * glm::sin(angle);

        planet->setPosition(glm::vec3{x, 0.0f, y} + offset);
        planet->setScale(glm::vec3{size});
        planet->setParent(anchor);
    }
}

void MainScene::setupShaders(Renderer& renderer){
    std::unique_ptr<ShaderProgram> sphereProgram = std::make_unique<ShaderProgram>();
    sphereProgram->addShader(FileReader::readFile("Shaders/SphereShader/vertex.glsl").c_str(), GL_VERTEX_SHADER);
    sphereProgram->addShader(FileReader::readFile("Shaders/SphereShader/fragment.glsl").c_str(), GL_FRAGMENT_SHADER);
    sphereProgram->link();
    renderer.addShaderProgram("sphereShader", std::move(sphereProgram));


    std::unique_ptr<ShaderProgram> sunProgram = std::make_unique<ShaderProgram>();
    sunProgram->addShader(FileReader::readFile("Shaders/SunShader/vertex.glsl").c_str(), GL_VERTEX_SHADER);
    sunProgram->addShader(FileReader::readFile("Shaders/SunShader/fragment.glsl").c_str(), GL_FRAGMENT_SHADER);
    sunProgram->link();
    renderer.addShaderProgram("sunShader", std::move(sunProgram));
}

void MainScene::onUpdate(Renderer& renderer, Window& window, float deltaT) {
    ObjectReference<PlanetBody> earth = getObjectByName<PlanetBody>("earth");

    ObjectView<PlanetBody> planets = getObjectsOfType<PlanetBody>();

    for(auto& planet : planets){
        planet.rotate(deltaT*0.5, glm::vec3(0.0f, 1.0f, 0.0f));
    }

}

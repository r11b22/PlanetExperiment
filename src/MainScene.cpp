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
#include "SkyMap.hpp"
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
    AssetLoadInfo<CubemapTexture> starSkybox = {"starSkyBox"};

    std::filesystem::path dir = "Textures/stars";

    starSkybox.setPath(CubeFace::Right,  dir / "px.png"); // Positive X
    starSkybox.setPath(CubeFace::Left,   dir / "nx.png"); // Negative X

    starSkybox.setPath(CubeFace::Top,    dir / "py.png"); // Positive Y
    starSkybox.setPath(CubeFace::Bottom, dir / "ny.png"); // Negative Y

    starSkybox.setPath(CubeFace::Front,  dir / "pz.png"); // Positive Z
    starSkybox.setPath(CubeFace::Back,   dir / "nz.png"); // Negative Z

    addAsset(starSkybox);
}

void MainScene::onLoad(Renderer& renderer, Window& window) {



    window.setVSYNC(false);
    renderer.setSetting(GL_TEXTURE_CUBE_MAP_SEAMLESS, true);

    setupShaders(renderer);

    InputManager* inputManager = new InputManager{window};

    ObjectReference<FirstPersonCamera> cam = createObject<FirstPersonCamera>("camera", inputManager, &window);


    ObjectReference<AmbientLight> ambientLight = createObject<AmbientLight>("light", glm::vec3{0.2f});

    PlanetGenerator sunGen{BASE_RESOLUTION};



    ObjectReference<PlanetBody> sun = sunGen.generatePlanet(*this);
    sun->setScale(glm::vec3{SUN_SCALE});
    sun->setShader("sunShader");



    ObjectReference<PointLight> sunLight = createObject<PointLight>("point", glm::vec3{1.0f});
    sunLight->setParent(sun);


    createPlanets(sun);



    cam->setPosition(glm::vec3{0.0f, 0.0f, 40.0f});

    ObjectReference<SkyMap> skyMap = createObject<SkyMap>("skymap", getAssetManager().getAssetByName<CubemapTexture>("starSkyBox"));
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

    std::unique_ptr<ShaderProgram> skyboxProgram = std::make_unique<ShaderProgram>();
    skyboxProgram->addShader(FileReader::readFile("Shaders/SkyboxShader/vertex.glsl").c_str(), GL_VERTEX_SHADER);
    skyboxProgram->addShader(FileReader::readFile("Shaders/SkyboxShader/fragment.glsl").c_str(), GL_FRAGMENT_SHADER);
    skyboxProgram->link();
    renderer.addShaderProgram("skyboxShader", std::move(skyboxProgram));
}

void MainScene::onUpdate(Renderer& renderer, Window& window, float deltaT) {
    ObjectReference<PlanetBody> earth = getObjectByName<PlanetBody>("earth");

    ObjectView<PlanetBody> planets = getObjectsOfType<PlanetBody>();

    for(auto& planet : planets){
        planet.rotate(deltaT*0.5, glm::vec3(0.0f, 1.0f, 0.0f));
    }

}

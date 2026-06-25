#include "MainScene.hpp"
#include "Defaults/Objects/Lighting/AmbientLight.h"
#include "Defaults/Objects/Lighting/DirectionalLight.h"
#include "InputManager.h"
#include "IcoSphere.hpp"
#include "Object/ObjectRepository.h"
#include "PlanetBody.hpp"
#include "Renderer/Renderer.h"

#include "Defaults/Camera/FirstPersonCamera.h"
#include "glm/ext/vector_float3.hpp"

void MainScene::onLoad(Renderer& renderer, Window& window) {
    window.setVSYNC(false);


    InputManager* inputManager = new InputManager{window};

    ObjectReference<FirstPersonCamera> cam = createObject<FirstPersonCamera>("camera", inputManager, &window);


    ObjectReference<AmbientLight> ambientLight = createObject<AmbientLight>("light");
    ObjectReference<DirectionalLight> directionalLight = createObject<DirectionalLight>("directional light", glm::vec3{1.0f, 1.0f, -1.0f}, glm::vec3{1.0f});


    ObjectReference<PlanetBody> planetBody = createObject<PlanetBody>("isosphere", Material{"yes", glm::vec3{1.0f, 0.0f, 0.0f}});

    cam->setPosition(glm::vec3{0.0f, 1.0f, 2.0f});
}

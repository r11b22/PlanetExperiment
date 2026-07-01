#pragma once

#include "Object/ObjectRepository.h"
#include "PlanetBody.hpp"
#include "Scene/Scene.hpp"

class MainScene : public Scene {
    private:
        static constexpr int BASE_RESOLUTION = 2048;
        static constexpr int SUN_SCALE = 10.0f;

        static constexpr int PLANET_COUNT = 10;
        static constexpr float DIST_VARIATION = 1.0f;
        static constexpr float SUN_PADDING = 10.0f;
        static constexpr float SIZE_DIST_RELATION = 3.0f;

        static constexpr float BASE_ROTATION_SPEED = 10.0f;
    public:
        MainScene();
        void onLoad(Renderer &renderer, Window &window) override;
        void onUpdate(Renderer& renderer, Window& window, float deltaT) override;
    private:
        void setupShaders(Renderer& renderer);
        void createPlanets(ObjectReference<PlanetBody> sun);
};

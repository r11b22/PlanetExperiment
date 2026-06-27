#pragma once

#include "Scene/Scene.hpp"

class MainScene : public Scene {
    private:
    public:
        MainScene();
        void onLoad(Renderer &renderer, Window &window) override;
        void onUpdate(Renderer& renderer, Window& window, float deltaT) override;
    private:
        void setupSphereShader(Renderer& renderer);
};

#pragma once

#include "IcoSphere.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "glm/ext/vector_float3.hpp"

class PlanetBody : public IcoSphere {
    private:
        CubemapTextureReference mNormalMap;
        CubemapTextureReference mDepthMap;

        float mDepthMultiplier = 0.1f;

        float mTopHeight = 0.60f;
        float mBottomHeight = 0.30f;

        std::vector<glm::vec3> mColors = {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f)};
    public:
        PlanetBody(std::string name, Material material, CubemapTextureReference normalMap, CubemapTextureReference depthMap);

        void setTopHeight(float height);
        void setBottomHeight(float height);
        void setColors(std::vector<glm::vec3> colors);

        void setDepthMultiplier(float mul);

        std::vector<RenderCommand> getRenderCommands() override;
    private:
};

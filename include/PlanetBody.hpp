#pragma once

#include "IcoSphere.hpp"
#include "Texture/CubemapTextureReference.hpp"

class PlanetBody : public IcoSphere {
    private:
        CubemapTextureReference mColorTexture;
        CubemapTextureReference mNormalMap;
        CubemapTextureReference mDepthMap;

        float mDepthMultiplier = 0.1f;
    public:
        PlanetBody(std::string name, Material material, CubemapTextureReference texture, CubemapTextureReference normalMap, CubemapTextureReference depthMap);

        void setDepthMultiplier(float mul);

        std::vector<RenderCommand> getRenderCommands() override;
    private:
};

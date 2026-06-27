#pragma once

#include "IcoSphere.hpp"
#include "Texture/CubemapTextureReference.hpp"

class PlanetBody : public IcoSphere {
    private:
        CubemapTextureReference mColorTexture;
        CubemapTextureReference mNormalMap;
    public:
        PlanetBody(std::string name, Material material, CubemapTextureReference texture, CubemapTextureReference normalMap);


        std::vector<RenderCommand> getRenderCommands() override;
    private:
};

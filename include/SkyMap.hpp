#pragma once


#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Renderer/RenderCommand.h"
#include "Texture/CubemapTextureReference.hpp"
#include <vector>
class SkyMap : public MeshObject {
    private:
        CubemapTextureReference mTexture;
    public:
        SkyMap(std::string name, CubemapTextureReference texture);
        void onLoad() override;
        std::vector<RenderCommand> getRenderCommands() override;
    private:
        void createCube();
};

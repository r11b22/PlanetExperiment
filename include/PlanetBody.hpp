#pragma once

#include "IcoSphere.hpp"

class PlanetBody : public IcoSphere {
    private:
        std::string mCubemapName;
        std::string mCubemapNormalName;
    public:
        PlanetBody(std::string name, Material material, std::string cubemapName, std::string cubemapNormalName);

        std::vector<RenderCommand> getRenderCommands() override;
    private:
};

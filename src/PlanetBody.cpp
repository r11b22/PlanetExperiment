#include "PlanetBody.hpp"
#include "Defaults/Objects/TransformableObject.h"
#include "IcoSphere.hpp"
#include "Renderer/RenderCommand.h"
#include <vector>



PlanetBody::PlanetBody(std::string name, Material material, std::string cubemapName, std::string cubemapNormalName)
    : TransformableObject(name), IcoSphere(name, material, 5), mCubemapName(cubemapName), mCubemapNormalName(cubemapNormalName)
{}


std::vector<RenderCommand> PlanetBody::getRenderCommands(){
    StateChangeCommand defaultState;

    DrawCommand command;

    command.material = getMaterial();
    command.mesh = getMesh();
    command.shaderName = getShader();

    glm::mat4 transformationMatrix = getTransformationMatrix();
    command.uniforms.push_back({"uModelMatrix", transformationMatrix});

    command.textureUniforms.push_back({"uCubemap", getAssetManager().getCubemapByName(mCubemapName)});
    command.textureUniforms.push_back({"uNormalCubemap", getAssetManager().getCubemapByName(mCubemapNormalName)});

    return {defaultState, command};
}

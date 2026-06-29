#include "PlanetBody.hpp"
#include "Defaults/Objects/TransformableObject.h"
#include "IcoSphere.hpp"
#include "Renderer/RenderCommand.h"
#include <vector>



PlanetBody::PlanetBody(std::string name, Material material, CubemapTextureReference normalMap, CubemapTextureReference depthMap)
    : TransformableObject(name), IcoSphere(name, material, 5), mNormalMap(normalMap), mDepthMap(depthMap)
{}


void PlanetBody::setDepthMultiplier(float mul){
    mDepthMultiplier = mul;
}

std::vector<RenderCommand> PlanetBody::getRenderCommands(){
    StateChangeCommand defaultState;

    DrawCommand command;

    command.material = getMaterial();
    command.mesh = getMesh();
    command.shaderName = getShader();

    glm::mat4 transformationMatrix = getTransformationMatrix();
    command.uniforms.push_back({"uModelMatrix", transformationMatrix});
    command.uniforms.push_back({"uDepthMultiplier", mDepthMultiplier});

    command.textureUniforms.push_back({"uNormalCubemap", mNormalMap});
    command.textureUniforms.push_back({"uDeptCubemap", mDepthMap});

    return {defaultState, command};
}

#include "PlanetBody.hpp"
#include "Defaults/Objects/TransformableObject.h"
#include "IcoSphere.hpp"
#include "Renderer/RenderCommand.h"
#include <vector>



PlanetBody::PlanetBody(std::string name, Material material, CubemapTextureReference texture, CubemapTextureReference normalMap)
    : TransformableObject(name), IcoSphere(name, material, 5), mColorTexture(texture), mNormalMap(normalMap)
{}


std::vector<RenderCommand> PlanetBody::getRenderCommands(){
    StateChangeCommand defaultState;

    DrawCommand command;

    command.material = getMaterial();
    command.mesh = getMesh();
    command.shaderName = getShader();

    glm::mat4 transformationMatrix = getTransformationMatrix();
    command.uniforms.push_back({"uModelMatrix", transformationMatrix});

    command.textureUniforms.push_back({"uCubemap", mColorTexture});
    command.textureUniforms.push_back({"uNormalCubemap", mNormalMap});

    return {defaultState, command};
}

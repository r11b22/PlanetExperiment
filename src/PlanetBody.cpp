#include "PlanetBody.hpp"
#include "Defaults/Objects/TransformableObject.h"
#include "IcoSphere.hpp"
#include "Renderer/RenderCommand.h"
#include "glm/ext/vector_float3.hpp"
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


    command.uniforms.push_back({"uTopHeight", mTopHeight});
    command.uniforms.push_back({"uBottomHeight", mBottomHeight});

    command.uniforms.push_back({"uColorCount", static_cast<int>(mColors.size())});

    for (int i = 0; i < mColors.size(); i++){
        command.uniforms.push_back({std::format("uColors[{}]", i), mColors[i]});
    }

    command.textureUniforms.push_back({"uNormalCubemap", mNormalMap});
    command.textureUniforms.push_back({"uDeptCubemap", mDepthMap});

    return {defaultState, command};
}

void PlanetBody::setColors(std::vector<glm::vec3> colors){
    mColors = colors;
}

void PlanetBody::setTopHeight(float height){
    mTopHeight = height;
}

void PlanetBody::setBottomHeight(float height){
    mBottomHeight = height;
}

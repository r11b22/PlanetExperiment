#include "SkyMap.hpp"
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Defaults/Objects/TransformableObject.h"
#include "Mesh/MeshReference.hpp"
#include "Renderer/RenderState.h"
#include "glm/ext/vector_float3.hpp"

SkyMap::SkyMap(std::string name, CubemapTextureReference texture)
    : TransformableObject(name), MeshObject(name, MeshReference(), Material("", glm::vec3{1.0f}))
    , mTexture(texture)
{
    setShader("skyboxShader");
}

std::vector<RenderCommand> SkyMap::getRenderCommands() {
    StateChangeCommand state;

    state.state.depthFunc = DepthFunc::LEqual;
    state.state.cullMode = CullMode::Front;

    DrawCommand command;

    command.material = getMaterial();
    command.mesh = getMesh();
    command.shaderName = getShader();

    command.textureUniforms.push_back({"uTexture", mTexture});

    return {state, command};
}

void SkyMap::onLoad(){
    createCube();
}

void SkyMap::createCube() {


    std::vector<float> localVertices = {
        // --- BACK FACE (Normal: 0, 0, -1) ---
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f, // Bottom-Left
            0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f, // Bottom-Right
            0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f, // Top-Right
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f, // Top-Left

        // --- FRONT FACE (Normal: 0, 0, 1) ---
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f, 0.0f, // Bottom-Left
            0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   1.0f, 0.0f, // Bottom-Right
            0.5f,  0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   1.0f, 1.0f, // Top-Right
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f, 1.0f, // Top-Left

        // --- LEFT FACE (Normal: -1, 0, 0) ---
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,   1.0f, 0.0f, // Top-Right
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f,  0.0f,   0.0f, 0.0f, // Top-Left
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f,  0.0f,   0.0f, 1.0f, // Bottom-Left
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,   1.0f, 1.0f, // Bottom-Right

        // --- RIGHT FACE (Normal: 1, 0, 0) ---
            0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  0.0f,   0.0f, 0.0f, // Top-Left
            0.5f,  0.5f, -0.5f,   1.0f, 0.0f,  0.0f,   1.0f, 0.0f, // Top-Right
            0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  0.0f,   1.0f, 1.0f, // Bottom-Right
            0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  0.0f,   0.0f, 1.0f, // Bottom-Left

        // --- BOTTOM FACE (Normal: 0, -1, 0) ---
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f, // Top-Left
            0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f, // Top-Right
            0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f, // Bottom-Right
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f, // Bottom-Left

        // --- TOP FACE (Normal: 0, 1, 0) ---
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f, // Bottom-Left
            0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f, // Bottom-Right
            0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-Right
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f  // Top-Left
    };

    std::vector<unsigned int> localIndices = {
        0,  2,  1,   0,  3,  2,  // Back Face
        4,  5,  6,   4,  6,  7,  // Front Face
        8,  9, 10,   8, 10, 11,  // Left Face
        12, 14, 13,  12, 15, 14,  // Right Face
        16, 17, 18,  16, 18, 19,  // Bottom Face
        20, 22, 21,  20, 23, 22   // Top Face
    };

    Mesh mesh{"cubemesh"};
    mesh.setVertices(localVertices);
    mesh.setIndices(localIndices);

    MeshReference ref = getAssetManager().addAsset(std::move(mesh));

    setMesh(ref);
}

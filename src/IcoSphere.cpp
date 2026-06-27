#include "IcoSphere.hpp"
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Defaults/Objects/TransformableObject.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/scalar_common.hpp"
#include "glm/ext/vector_float3.hpp"
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <vector>

IcoSphere::IcoSphere(std::string name, Material material, int subdivisions)
    : TransformableObject(name), MeshObject(name, {}, material)
{
    setShader("sphereShader");
    mVertices = createBaseVertices();

    mIndices = createBaseIndices();

    for (int i = 0; i < subdivisions; i++){
        subdivide(mVertices, mIndices);
    }
}


void IcoSphere::onLoad() {
    Mesh mesh{"cube"};

    mesh.setVertices(mVertices);
    mesh.setIndices(mIndices);

    MeshReference ref = getAssetManager().addMesh(std::move(mesh));
    setMesh(ref);
}


std::vector<float> IcoSphere::createBaseVertices(){
    float t = (1.0 + glm::sqrt(5.0)) / 2.0;

    float length = glm::sqrt(1.0f + t * t);

    float a = 1.0f/length;
    float b = t/length;

    std::vector<float> vertices = {};

    const int GROUPS = 3;
    const int NORMAL_START = 3;
    const int UV_START = 6;
    int offset = 0;

    vertices.resize(12 * VERTEX_SIZE);
    for (int i = 0; i < GROUPS; i++){
        int aInd = i;
        int bInd = (i + 1) % 3;
        int zeroInd = (i + 2) % 3;

        for (int bSign = 0; bSign < 2; bSign++){
            int minB = bSign == 0 ? 1 : -1;

            for (int aSign = 0; aSign < 2; aSign++){
                int minA = aSign == 0 ? -1 : 1;

                vertices[aInd + offset] = a * minA;
                vertices[bInd + offset] = b * minB;
                vertices[zeroInd + offset] = 0.0f;

                vertices[NORMAL_START + aInd + offset] = a * minA;
                vertices[NORMAL_START + bInd + offset] = b * minB;
                vertices[NORMAL_START + zeroInd + offset] = 0.0f;

                vertices[UV_START + offset] = 0.0f;
                vertices[UV_START + 1 + offset] = 0.0f;


                offset += 8;
            }
        }
    }

    return vertices;
}

std::vector<unsigned int> IcoSphere::createBaseIndices() {
    return {
           0, 11,  5,
           0,  5,  1,
           0,  1,  7,
           0,  7, 10,
           0, 10, 11,

           1,  5,  9,
           5, 11,  4,
          11, 10,  2,
          10,  7,  6,
           7,  1,  8,

           3,  9,  4,
           3,  4,  2,
           3,  2,  6,
           3,  6,  8,
           3,  8,  9,

           4,  9,  5,
           2,  4, 11,
           6,  2, 10,
           8,  6,  7,
           9,  8,  1
       };
}

void IcoSphere::subdivide(std::vector<float> vertices, std::vector<unsigned int> indices){
    std::vector<unsigned int> newIndices{};

    const int faces = indices.size() / VERTICES_PER_FACE;

    std::unordered_map<EdgeKey, int> midpointCache = {};


    for (int face = 0; face < faces; face++){
        // subidivide one face
        int point1Index = indices[face * VERTICES_PER_FACE];
        int point2Index = indices[face * VERTICES_PER_FACE + 1];
        int point3Index = indices[face * VERTICES_PER_FACE + 2];

        int point1Start = point1Index * VERTEX_SIZE;
        int point2Start = point2Index * VERTEX_SIZE;
        int point3Start = point3Index * VERTEX_SIZE;

        int mid1 = createEdgeMidpoint(point1Index, point2Index, vertices, midpointCache);
        int mid2 = createEdgeMidpoint(point2Index, point3Index, vertices, midpointCache);
        int mid3 = createEdgeMidpoint(point3Index, point1Index, vertices, midpointCache);

        newIndices.push_back(point1Index);
        newIndices.push_back(mid1);
        newIndices.push_back(mid3);

        newIndices.push_back(point2Index);
        newIndices.push_back(mid2);
        newIndices.push_back(mid1);

        newIndices.push_back(point3Index);
        newIndices.push_back(mid3);
        newIndices.push_back(mid2);

        newIndices.push_back(mid1);
        newIndices.push_back(mid2);
        newIndices.push_back(mid3);
    }

    mVertices = vertices;
    mIndices = newIndices;
}

int IcoSphere::createEdgeMidpoint(int pointA, int pointB, std::vector<float>& vertices, std::unordered_map<EdgeKey, int>& cache){
    EdgeKey key{pointA, pointB};

    auto it = cache.find(key);

    if (it == cache.end()){
        // create a new midpoint
        int pointAStart = pointA * VERTEX_SIZE;
        glm::vec3 posA {vertices[pointAStart], vertices[pointAStart + 1], vertices[pointAStart + 2]};

        int pointBStart = pointB * VERTEX_SIZE;
        glm::vec3 posB {vertices[pointBStart], vertices[pointBStart + 1], vertices[pointBStart + 2]};

        glm::vec3 mid = midpoint(posA, posB);

        // correct distance
        mid = glm::normalize(mid);

        // vertex
        vertices.push_back(mid.x);
        vertices.push_back(mid.y);
        vertices.push_back(mid.z);

        //normals
        vertices.push_back(mid.x);
        vertices.push_back(mid.y);
        vertices.push_back(mid.z);

        //uv
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        int idx = (vertices.size() / VERTEX_SIZE) - 1;

        // add to cache
        cache.emplace(key, idx);

        return idx;
    }else{
        return it->second;
    }
}

glm::vec3 IcoSphere::midpoint(glm::vec3 pointA, glm::vec3 pointB){
    return (pointA + pointB) / 2.0f;
}

EdgeKey::EdgeKey(int a, int b)
{
    v1 = std::min(a, b);
    v2 = std::max(a, b);
}

bool EdgeKey::operator==(const EdgeKey& other) const {
    return v1 == other.v1 && v2 == other.v2;
}

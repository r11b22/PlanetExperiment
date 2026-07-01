#pragma once


#include "Defaults/Objects/Drawables/MeshObject.h"
#include "glm/ext/vector_float3.hpp"
#include <vector>


struct EdgeKey {
    int v1;
    int v2;

    EdgeKey(int a, int b);

    bool operator==(const EdgeKey& other) const;
};

// specialization of std::hash into the std namespace
namespace std {
    template <>
    struct hash<EdgeKey> {
        std::size_t operator()(const EdgeKey& edge) const noexcept {
            // A classic, fast hash-combining technique (similar to boost::hash_combine)
            std::size_t h1 = std::hash<int>{}(edge.v1);
            std::size_t h2 = std::hash<int>{}(edge.v2);

            // 0x9e3779b9 is the golden ratio constant, preventing bad hashing patterns
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
}

class IcoSphere : public MeshObject {
    private:
        int mSubdivisions = 0;




        const static int VERTEX_SIZE = 8;
        const static int VERTICES_PER_FACE = 3;
    public:
        IcoSphere(std::string name, Material material, int subdivisions);

        void onLoad() override;
    private:
        std::vector<float> createBaseVertices();
        std::vector<unsigned int>  createBaseIndices();
        void subdivide(std::vector<float>& vertices, std::vector<unsigned int>& indices);
        /**
         * Gets the index for the vertex of the point between the 2 given points, If the point does not yet exists: create a new one
         */
        int createEdgeMidpoint(int pointA, int pointB, std::vector<float>& vertices, std::unordered_map<EdgeKey, int>& cache);
        glm::vec3 midpoint(glm::vec3 pointA, glm::vec3 pointB);
};

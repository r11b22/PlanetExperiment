#pragma once

#include "Object/ObjectRepository.h"
#include "PlanetBody.hpp"

class PlanetGenerator {
    private:
        int mSize;
    public:
        PlanetGenerator(int size);

        ObjectReference<PlanetBody> generatePlanet(Scene& scene);
    private:
};

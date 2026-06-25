#include "PlanetBody.hpp"
#include "Defaults/Objects/TransformableObject.h"
#include "IcoSphere.hpp"



PlanetBody::PlanetBody(std::string name, Material material)
    : TransformableObject(name), IcoSphere(name, material, 4)
{}

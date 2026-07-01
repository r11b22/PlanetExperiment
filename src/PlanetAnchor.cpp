#include "PlanetAnchor.hpp"
#include "Defaults/Objects/TransformableObject.h"
#include "glm/trigonometric.hpp"

PlanetAnchor::PlanetAnchor(std::string name, float speed)
    : TransformableObject(name), mSpeed(speed)
    {}


void PlanetAnchor::onUpdate(float deltaT) {
    rotate(glm::radians(deltaT * mSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
}

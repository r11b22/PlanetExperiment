#pragma once

#include "Defaults/Objects/TransformableObject.h"
class PlanetAnchor : public TransformableObject{
    private:
        float mSpeed;
    public:
        // speed in deg/sec
        PlanetAnchor(std::string name, float speed);

        void onUpdate(float deltaT) override;
    private:
};

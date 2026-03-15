//
// Created by milosz on 3/12/26.
//
#include "CelestialBody.h"
#include <vector>

class GravityEngine {
private:
    std::vector<CelestialBody> bodies;
    const float G = 6.67430e-11f;
public:
    void addBody(const CelestialBody& body);
    void update(float deltaTime);
    const std::vector<CelestialBody>& getBodies() const;
};

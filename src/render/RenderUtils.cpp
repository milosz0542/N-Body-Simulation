#include "RenderUtils.h"

std::vector<float> RenderUtils::preparePositionBuffer(const std::vector<CelestialBody> &bodies) {
    std::vector<float> buffer(bodies.size() * 3);

    for (size_t i = 0; i < bodies.size(); ++i) {
        Eigen::Vector3f pos = bodies[i].position;
        buffer[i * 3 + 0] = pos.x();
        buffer[i * 3 + 1] = pos.y();
        buffer[i * 3 + 2] = pos.z();
    }

    return buffer;
};

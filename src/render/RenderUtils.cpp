#include "RenderUtils.h"

std::vector<float> RenderUtils::preparePositionBuffer(const std::vector<CelestialBody> &bodies) {
    std::vector<float> buffer(bodies.size() * 5);

    for (size_t i = 0; i < bodies.size(); ++i) {
        Eigen::Vector3f pos = bodies[i].position;
        Eigen::Vector3f vel = bodies[i].velocity;

        buffer[i * 5 + 0] = pos.x();
        buffer[i * 5 + 1] = pos.y();
        buffer[i * 5 + 2] = pos.z();

        buffer[i * 5 + 3] = bodies[i].radius; // radius of body

        buffer[i * 5 + 4] = vel.norm(); // length of velocity vector
    }

    return buffer;
}

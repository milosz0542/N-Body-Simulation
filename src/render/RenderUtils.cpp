#include "RenderUtils.h"

void RenderUtils::fillPositionBuffer(const std::vector<CelestialBody> &bodies, std::vector<GLfloat>& outBuffer) {
    outBuffer.clear();

    if (outBuffer.capacity() < bodies.size() * 5) {
        outBuffer.reserve(bodies.size() * 5);
    }

    for (const auto& body : bodies) {
        outBuffer.push_back(body.position.x());
        outBuffer.push_back(body.position.y());
        outBuffer.push_back(body.position.z());
        outBuffer.push_back(body.radius);
        outBuffer.push_back(body.velocity.norm());
    }
}

//
// Created by milosz on 3/6/26.
//

#include "CelestialBody.h"

CelestialBody::CelestialBody()
    : position(Eigen::Vector3f::Zero()), velocity(Eigen::Vector3f::Zero()), force(Eigen::Vector3f::Zero()), mass(1.0f), radius(1.0f) {}

CelestialBody::CelestialBody(const Eigen::Vector3f& pos, const Eigen::Vector3f& vel, float m)
    : position(pos), velocity(vel), force(Eigen::Vector3f::Zero()), mass(m), radius(calculateRadius(m)) {}

float CelestialBody::calculateRadius(float mass) {
    if (mass <= 0) return 0.0f;
    return std::cbrt(mass); // Temporary approximation of radius (If we assume that density is constance for every body, then R is proportional to cbrt(m))
}

void CelestialBody::addForce(Eigen::Vector3f& force) { this->force += force; }

// void CelestialBody::update(float deltaTime) {
//     // Count acceleration (F/m)
//     Eigen::Vector3f acceleration = force / mass;
//
//     // v = v0 + a*t
//     velocity += acceleration * deltaTime;
//
//     // x = x0 + v*t
//     position += velocity * deltaTime;
// }

void CelestialBody::updatePosition(float deltaTime) {
    Eigen::Vector3f acceleration = force / mass;

    // Step 1: Position update
    position += velocity * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;

    // Step 2: Velocity update
    velocity += 0.5f * acceleration * deltaTime;
}

void CelestialBody::updateVelocity(float deltaTime) {
    Eigen::Vector3f newAcceleration = force / mass;

    velocity += 0.5f * newAcceleration * deltaTime;
}
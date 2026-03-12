//
// Created by milosz on 3/6/26.
//
#pragma once
#include <Eigen/Dense>

class CelestialBody {
public:
    Eigen::Vector3f position;
    Eigen::Vector3f velocity;
    Eigen::Vector3f force;
    float mass;
    float radius;

    CelestialBody();
    CelestialBody(const Eigen::Vector3f& pos, const Eigen::Vector3f& vel, float m);

    void addForce(Eigen::Vector3f& force);

    void resetForce() { force.setZero(); }

    // [[nodiscard]] Eigen::Vector3f getPosition() const { return position; }
    // [[nodiscard]] Eigen::Vector3f getVelocity() const { return velocity; }
    // [[nodiscard]] Eigen::Vector3f getForce() const { return force; }
    // [[nodiscard]] float getMass() const { return mass; }
    // [[nodiscard]] float getRadius() const { return radius; }

private:
    static float calculateRadius(float mass);
    // Eigen::Vector3f position;
    // Eigen::Vector3f velocity;
    // Eigen::Vector3f force;
    // float mass;
    // float radius;
};
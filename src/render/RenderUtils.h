//
// Created by milosz on 3/22/26.
//

#pragma once
#include <vector>
#include <GL/gl.h>
#include "core/CelestialBody.h"


class RenderUtils {
public:
    static void fillPositionBuffer(const std::vector<CelestialBody>& bodies, std::vector<GLfloat>& outBuffer);
};

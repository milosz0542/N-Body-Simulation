//
// Created by milosz on 3/22/26.
//

#pragma once
#include <vector>
#include "core/CelestialBody.h"


class RenderUtils {
public:
    static std::vector<float> preparePositionBuffer(const std::vector<CelestialBody>& bodies);
};

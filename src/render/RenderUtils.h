/**
* @file RenderUtils.h
 * @brief Rendering helper utilities for preparing GPU-friendly simulation data.
 *
 * This file declares utility functions used to transform physics-domain objects
 * (e.g., @ref CelestialBody) into flat buffers suitable for OpenGL upload.
 */
#pragma once

#include <vector>
#include <GL/gl.h>

#include "core/CelestialBody.h"

/**
 * @class RenderUtils
 * @brief Static helper class for rendering data preparation.
 *
 * RenderUtils provides convenience routines that convert simulation body data
 * into contiguous attribute arrays (e.g., positions/radii/speeds) expected by
 * rendering code and shader input layouts.
 *
 * The class is intentionally stateless and exposes only static methods.
 */
class RenderUtils {
public:
    /**
     * @brief Fill a flat OpenGL float buffer with per-body render attributes.
     * @param bodies Input list of celestial bodies from simulation.
     * @param outBuffer Destination CPU buffer to be written with packed attributes.
     *
     * The exact attribute layout/order in @p outBuffer is defined by the
     * implementation and must match VBO attribute configuration in render code.
     */
    static void fillPositionBuffer(const std::vector<CelestialBody>& bodies,
                                   std::vector<GLfloat>& outBuffer);
};
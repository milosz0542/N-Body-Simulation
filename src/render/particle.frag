#version 330 core

in float vSpeed;

// Out pixel color
out vec4 FragColor;

uniform bool useVelocityColor; // Whether to color particles based on speed

void main() {
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0; // Vector from center of point sprite to current fragment
    float distSq = dot(circCoord, circCoord); // Squared distance from center
    if (distSq > 1.0) {
        discard; // Ignore fragments outside the circle (point sprite is a square, we want a circle)
    }

    vec3 baseColor;
    if (useVelocityColor) {
        float speedFactor = clamp(vSpeed / 50.0, 0.0, 1.0); // Normalize speed to [0, 1] range for color modulation

        vec3 slowColor = vec3(0.1, 0.4, 1.0); // Color for slow particles (blueish)
        vec3 mediumColor = vec3(1.0, 0.8, 0.2); // Color for medium speed particles (orange)
        vec3 fastColor = vec3(1.0, 0.1, 0.1); // Color for fast particles (reddish)

        if (speedFactor < 0.5) {
            baseColor = mix(slowColor, mediumColor, speedFactor * 2.0); // Blend from slow to medium color
        } else {
            baseColor = mix(mediumColor, fastColor, (speedFactor - 0.5) * 2.0); // Blend from medium to fast color
        }
    } else {
        baseColor = vec3(1.0, 1.0, 0.5); // Default color (yellowish) if not using velocity-based coloring
    }

    float alpha = 1.0-distSq; // Alpha decreases with distance from center (fades out towards edges)
    alpha = pow(alpha, 1.5);

    FragColor = vec4(baseColor, alpha);
}
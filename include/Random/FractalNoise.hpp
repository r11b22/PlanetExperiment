#pragma once


#include "Random/PerlinNoise.hpp"
class FractalNoise {
    private:
        PerlinNoise mGenerator;
    public:
        FractalNoise(int seed);

        double noise(double x, double y, double z, int octaves, double lacunarity, double gain) {
            double total = 0.0;
            double frequency = 1.0;
            double amplitude = 1.0;
            double maxValue = 0.0;  // Used for normalizing the result

            for (int i = 0; i < octaves; i++) {
                total += mGenerator.noise(x * frequency, y * frequency, z * frequency) * amplitude;

                maxValue += amplitude;

                frequency *= lacunarity; // Lacunarity typically = 2.0 (doubles frequency)
                amplitude *= gain;       // Gain typically = 0.5 (halves amplitude)
            }

            // Normalize back to [-1, 1]
            return total / maxValue;
        }
    private:
};

#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

class PerlinNoise {
private:
    std::vector<int> p;

    double fade(double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    double grad(int hash, double x, double y, double z) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    PerlinNoise(unsigned int seed = 0) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);

        std::mt19937 engine(seed);
        std::shuffle(p.begin(), p.end(), engine);

        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x, double y, double z) {
        // Find unit cube coordinates
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        int Z = static_cast<int>(std::floor(z)) & 255;

        // Find relative coordinates inside the cube
        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);

        // Compute fade curves
        double u = fade(x);
        double v = fade(y);
        double w = fade(z);

        // Hash coordinates of the 8 cube corners
        int A  = p[X] + Y;
        int AA = p[A] + Z;
        int AB = p[A + 1] + Z;
        int B  = p[X + 1] + Y;
        int BA = p[B] + Z;
        int BB = p[B + 1] + Z;

        // Blend the results using lerp
        return lerp(w, lerp(v, lerp(u, grad(p[AA],     x,     y,     z),
                                       grad(p[BA],     x - 1, y,     z)),
                               lerp(u, grad(p[AB],     x,     y - 1, z),
                                       grad(p[BB],     x - 1, y - 1, z))),
                       lerp(v, lerp(u, grad(p[AA + 1], x,     y,     z - 1),
                                       grad(p[BA + 1], x - 1, y,     z - 1)),
                               lerp(u, grad(p[AB + 1], x,     y - 1, z - 1),
                                       grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }
};

#pragma once
#include "PerlinNoise.hpp"
#include <vector>
#include <iostream>

class noise {
public:
    siv::PerlinNoise::seed_type Seed; // needs to be randomized
    siv::PerlinNoise Perlin;
    std::vector<std::vector<double>> GenerateChunk(siv::PerlinNoise& Perlin, int PositionX = 0, int PositionY = 0, int Size = 16, double Scale = 0.1);

    void Initialize()
    {
        std::cout << "called" << std::endl;
        Seed = 12345;
        Perlin = siv::PerlinNoise(Seed);
        GenerateChunk(Perlin);
    }
};
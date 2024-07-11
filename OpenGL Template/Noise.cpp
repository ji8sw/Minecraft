#include "Noise.h"

std::vector<std::vector<double>> noise::GenerateChunk(siv::PerlinNoise& Perlin, int PositionX, int PositionY, int Size, double Scale) {
    std::vector<std::vector<double>> ChunkInfo(Size, std::vector<double>(Size));

    for (int Y = 0; Y < Size; ++Y) {
        for (int X = 0; X < Size; ++X) {
            double WorldX = (PositionX * Size + X) * Scale;
            double WorldY = (PositionY * Size + Y) * Scale;
            double NoiseValue = Perlin.noise2D(WorldX, WorldY);
            ChunkInfo[X][Y] = NoiseValue;
        }
    }

    return ChunkInfo;
}
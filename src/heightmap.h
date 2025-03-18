#pragma once

#include "common.h"

struct Heightmap {
    Heightmap(IntPoint size);
    ~Heightmap();

    HRESULT load_from_file(const char* filename);
    HRESULT create_random_heightmap(int seed, float noise_size, float persistence, int octaves);
    float get_height(IntPoint p);

    IntPoint size;
    float max_height;
    float* heightmap;
};
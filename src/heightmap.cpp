#include "heightmap.h"

/* return: [-1.0f, 1.0f] */
float noise(int x) {
    // TODO: better noise func?
    x = (x<<13) ^ x;
    return (1.0f - ((x * (x*x * 15731 + 789221)
        + 1376312589) &
        0x7fffffff) / 1073741824.0f);
}

/* cosine interpolation */
float cos_interp(float x, float y, float a) {
    float angle = a * PI;
    float pct = (1.0f - cos(angle)) * 0.5f;
    return x * (1.0f - pct) + y * pct;
}

Heightmap::Heightmap(IntPoint in_size) : size(in_size), max_height(0) {
    heightmap = new float[size.x * size.y];
    memset(heightmap, 0, size.x * size.y * sizeof(float));
}

Heightmap::~Heightmap() {
    delete[] heightmap;
}

HRESULT Heightmap::load_from_file(const char* filename) {
    return E_NOTIMPL;
}

HRESULT Heightmap::create_random_heightmap(int seed, float noise_size, float persistence, int octaves) {
    for (int x=0; x < size.x; ++x) {
        for (int y=0; y < size.y; ++y) {
            float xf = ((float)x / (float)size.x) * noise_size;
            float yf = ((float)y / (float)size.y) * noise_size;

            float height = 0;
            for (int oct=0; oct<octaves; ++oct) {
                float freq = pow(2, oct);
                float amp = pow(persistence, oct);

                float tx = xf * freq;
                float ty = yf * freq;

                float v1 = noise((int)tx + (int)ty * 57 + seed);
                float v2 = noise((int)tx + 1 + (int)ty * 57 + seed);
                float v3 = noise((int)tx + (int)(ty+1) * 57 + seed);
                float v4 = noise((int)tx + 1 + (int)(ty+1) * 57 + seed);

                // smooth in x axis
                float i1 = cos_interp(v1, v2, tx - (int)tx);
                float i2 = cos_interp(v2, v3, tx - (int)tx);

                // smooth in y axis
                height += cos_interp(i1, i2, ty - (int)ty);
            }

            heightmap[x + y * size.x] = height;
        }
    }
    return E_NOTIMPL;
}

float Heightmap::get_height(IntPoint p) {
    return heightmap[p.y * size.x + p.x];
}


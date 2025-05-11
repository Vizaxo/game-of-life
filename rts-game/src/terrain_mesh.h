#pragma once

#include <vector>

#include "common.h"
#include "heightmap.h"

struct TerrainVert {
    XMFLOAT3 pos;
    XMFLOAT3 normal;
    XMFLOAT4 color;
};

struct Patch {
    HRESULT create_mesh(Heightmap& hm, RECT src, ID3D11Device* device);
    void render();
    static void setup_patches(ID3D11Device* device);

    friend struct Terrain;
private:
    IntPoint size {0,0};
    int num_vertices;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    static ID3D11VertexShader* vs;
    static ID3DBlob* vs_bytecode;
    static ID3D11PixelShader* ps;
};

struct Terrain {
    Terrain(IntPoint size) : size(size), heightmap({size.x+1,size.y+1}) {}
    void generate_random_terrain(int seed, int num_patches);
    void create_patches(int num_patches);
    void render();

    ID3D11InputLayout* input_layout;
    IntPoint size;
    Heightmap heightmap;
    std::vector<Patch> patches;
};

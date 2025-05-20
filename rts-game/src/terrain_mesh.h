#pragma once

#include <vector>

#include "common.h"
#include "heightmap.h"
#include "renderer/renderer.h"

struct TerrainVert {
    XMFLOAT3 pos;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
};

struct Patch {
    HRESULT create_mesh(Heightmap& hm, RECT src, ID3D11Device* device);
    void render(RenderState rs);
    static void setup_patches(ID3D11Device* device);

    friend struct Terrain;
private:
    IntPoint size {0,0};
    IntPoint position {0,0};
    int num_vertices;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    static ID3D11VertexShader* vs;
    static ID3DBlob* vs_bytecode;
    static ID3D11PixelShader* ps;
    static ID3D11Buffer* view_cb;
	static ID3D11RasterizerState* rasterizer_state;
};

struct Terrain {
    Terrain(IntPoint size) : size(size), heightmap({size.x+1,size.y+1}) {}
    void generate_random_terrain(int seed, int num_patches);
    void create_patches(int num_patches);
    void render(RenderState rs);

    ID3D11ShaderResourceView* grass_srv;
    ID3D11ShaderResourceView* water_srv;
    ID3D11ShaderResourceView* stone_srv;
    ID3D11InputLayout* input_layout;
    IntPoint size;
    Heightmap heightmap;
    std::vector<Patch> patches;
};

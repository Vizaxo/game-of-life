#pragma once

#include "common.h"
#include "heightmap.h"

struct TerrainVert {
    XMFLOAT3 pos;
    XMFLOAT3 normal;
    XMFLOAT4 color;
};

struct Patch {
    HRESULT create_mesh(Heightmap& hm, RECT src, ID3D11Device* device, int index);
    void render(ID3D11Device* device);

private:
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
};

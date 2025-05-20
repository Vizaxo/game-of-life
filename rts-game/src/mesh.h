#pragma once

#include "common.h"
#include "renderer/renderer.h"
#include <vector>

struct MeshVert {
    XMFLOAT3 pos;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
};

struct MeshData {
    std::vector<MeshVert> verts;
    std::vector<u32> indices;
    MeshData(std::vector<MeshVert>&& verts, std::vector<u32>&& indices) {
        this->verts = std::move(verts);
        this->indices = std::move(indices);
    }
};

class Mesh {
public:
	Mesh(const char* name);
	HRESULT load(const char* name);
	void render(RenderState rs);
	void release();
    static void static_init();

private:
    int num_vertices;
    int num_indices;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    XMFLOAT3 position;

    static ID3D11VertexShader* vs;
    static ID3DBlob* vs_bytecode;
    static ID3D11PixelShader* ps;
    static ID3D11Buffer* view_cb;
    static ID3D11InputLayout* input_layout;
	static ID3D11RasterizerState* rasterizer_state;

    std::vector<ID3D11ShaderResourceView*> textures;
};

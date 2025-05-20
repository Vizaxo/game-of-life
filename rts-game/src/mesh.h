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
    friend struct MeshInstance;
public:
	Mesh(const char* name);
	HRESULT load(const char* name);
	void release();
    static void static_init();

private:
    int num_vertices;
    int num_indices;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;

    static ID3D11VertexShader* vs;
    static ID3DBlob* vs_bytecode;
    static ID3D11PixelShader* ps;
    static ID3D11Buffer* view_cb;
    static ID3D11InputLayout* input_layout;
	static ID3D11RasterizerState* rasterizer_state;

    std::vector<ID3D11ShaderResourceView*> textures;
};

struct MeshInstance {
	void render(RenderState rs);
    MeshInstance(Mesh* mesh, XMFLOAT3 pos) : mesh(mesh), position(pos) {}
    Mesh* mesh;
    XMFLOAT3 position;
    XMFLOAT3 rot = {0.f, 0.f, 0.f};
    XMFLOAT3 scale = {1.f, 1.f, 1.f};
};

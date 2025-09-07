#pragma once

#include "common.h"
#include "renderer.h"
#include "shader.h"
#include <vector>

struct MeshVert {
    XMFLOAT2 pos;
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

inline MeshData quad_mesh_data = {
    std::vector<MeshVert>({
        MeshVert({{0.f,0.f}, {0.f,0.f}}),
        MeshVert({{0.f,1.f}, {0.f,1.f}}),
        MeshVert({{1.f,0.f}, {1.f,0.f}}),
        MeshVert({{1.f,1.f}, {1.f,1.f}}),
	}),
    std::vector<u32>({0,1,2,1,3,2})
};

class Mesh {
    friend struct MeshInstance;
public:
	Mesh();
	HRESULT load(const char* name, MeshData& mesh_data);
	void release();
    static void static_init();

private:
    int num_vertices;
    int num_indices;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;

    struct Shader* vs;
    struct Shader* ps;
    ID3D11InputLayout* input_layout;

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

extern std::unique_ptr<Mesh> quad_mesh;

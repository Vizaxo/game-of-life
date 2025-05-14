#pragma once

#include "common.h"
#include <vector>

class Mesh {
public:
	Mesh(const char* name);
	~Mesh();
	HRESULT load(const char* name);
	void render();
	void release();

private:
    int num_vertices;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    static ID3D11VertexShader* vs;
    static ID3DBlob* vs_bytecode;
    static ID3D11PixelShader* ps;
    static ID3D11Buffer* view_cb;
	static ID3D11RasterizerState* rasterizer_state;

    std::vector<ID3D11ShaderResourceView*> textures;
};

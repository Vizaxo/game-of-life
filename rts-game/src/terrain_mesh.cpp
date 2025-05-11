#include "terrain_mesh.h"

#include <vector>

#include "dxutils.h"
#include "renderer/renderer.h"

// Patches

inline HRESULT Patch::create_mesh(Heightmap& hm, RECT src, ID3D11Device* device) {
    int width = src.right - src.left;
    int height = src.bottom - src.top;
    size.x = width;
    size.y = height;
    num_vertices = (width+1) * (height+1);
    int num_triangles = width * height * 2;

    // Create vertex buffer
    {
        D3D11_BUFFER_DESC bd {};
        bd.ByteWidth = sizeof(TerrainVert) * num_vertices;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = sizeof(TerrainVert);

        std::vector<TerrainVert> verts;
        verts.resize(num_vertices);
        for (int x = 0; x <= width; x++) {
            for (int y = 0; y <= height; y++) {
                TerrainVert& vert = verts[x + y*(width+1)];
                vert.pos = XMFLOAT3((float)x, (float)y, 0.0f); // TODO: put heightmap height back hm.get_height({x, y}));
                vert.normal = XMFLOAT3(0.0, 1.0, 0.0); // TODO: calc normals
                XMStoreFloat4(&vert.color, Colors::Green);
            }
        }
    
        D3D11_SUBRESOURCE_DATA vert_data {};
        vert_data.pSysMem = verts.data();
        vert_data.SysMemPitch = sizeof(TerrainVert);
        device->CreateBuffer(&bd, &vert_data, &vb);
    }

    // Create index buffer
    {
        int num_indices = num_triangles * 3;
        D3D11_BUFFER_DESC bd {};
        bd.ByteWidth = sizeof(u16) * num_indices;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = sizeof(u16);

        std::vector<u16> indices;
        indices.resize(num_indices);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                /* per quad
                2------3
                |    / *
                |  /   *
                0------1
                Prims: 0,3,1 // 2,3,0
                
                6------7------8
                |    / *    / *
                |  /   *  /   *
                3------4------5
                |    / *    / *
                | /    *  /   *
                0------1------2
                */
                int quad_index = x + y*width;
                int v0 = x + y * (width+1);
                int v1 = x+1 + y * (width+1);
                int v2 = x + (y+1) * (width+1);
                int v3 = x+1 + (y+1) * (width+1);
                
                indices[quad_index*6 + 0] = v0;
                indices[quad_index*6 + 1] = v3;
                indices[quad_index*6 + 2] = v1;
                indices[quad_index*6 + 3] = v2;
                indices[quad_index*6 + 4] = v3;
                indices[quad_index*6 + 5] = v0;
            }
        }

        D3D11_SUBRESOURCE_DATA index_data {};
        index_data.pSysMem = indices.data();
        device->CreateBuffer(&bd, &index_data, &ib);
    }

    // TODO: vertex attribute information. how are we storing this in DX11?

    return S_OK;
}

ID3D11VertexShader* Patch::vs;
ID3DBlob* Patch::vs_bytecode;
ID3D11PixelShader* Patch::ps;

void Patch::setup_patches(ID3D11Device* device) {
    compile_vertex_shader(device, L"terrain.hlsl", "main_vs", &vs, &vs_bytecode);
    compile_pixel_shader(device, L"terrain.hlsl", "main_ps", &ps);
}

inline void Patch::render() {
    u32 stride = sizeof(TerrainVert);
    u32 offset = 0;
    context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    context->IASetIndexBuffer(ib, DXGI_FORMAT_R16_UINT, 0);

    context->VSSetShader(vs, nullptr, 0);
    //context->VSSetConstantBuffers(0, 1, )

    context->PSSetShader(ps, nullptr, 0);

    context->DrawIndexed(size.x*size.y*6, 0, 0);
}


// Terrain

void Terrain::generate_random_terrain(int seed, int num_patches) {
    heightmap.create_random_heightmap(seed, 1.0, 0.5, 5);
}

void Terrain::create_patches(int num_patches) {
            /*
    for (int x = 0; x < num_patches; ++x) {
        for (int y = 0; y < num_patches; ++y) {
            RECT rect = {
                x * (size.x-1) / num_patches,
                y * (size.y-1) / num_patches,
                (x+1) * (size.x-1) / num_patches,
                (y+1) * (size.y-1) / num_patches,
            };
            patches.emplace_back();
            patches[x + y*num_patches].create_mesh(heightmap, rect, device);
        }
    }
            */
	patches.emplace_back();
    RECT rect = { 0, 0, size.x, size.y };
	patches[0].create_mesh(heightmap, rect, device);

    D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
        {"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    HRASSERT(device->CreateInputLayout(input_layout_desc, NUM_ELEMENTS(input_layout_desc), Patch::vs_bytecode->GetBufferPointer(), Patch::vs_bytecode->GetBufferSize(), &input_layout));
    assert(input_layout);
}

void Terrain::render() {
    context->IASetInputLayout(input_layout);
    // TODO: separate patches, separate positions
    patches[0].render();
}

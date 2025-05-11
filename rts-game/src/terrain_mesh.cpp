#include "terrain_mesh.h"

#include <vector>

#include "dxutils.h"

inline HRESULT Patch::create_mesh(Heightmap& hm, RECT src, ID3D11Device* device, int index) {
    int width = src.right - src.left;
    int height = src.bottom - src.top;
    int num_vertices = (width+1) * (height+1);
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
        verts.reserve(num_vertices);
        for (int x = 0; x <= width; x++) {
            for (int y = 0; y <= height; y++) {
                TerrainVert& vert = verts[x + y*(width+1)];
                vert.pos = XMFLOAT3((float)x, (float)y, hm.get_height({x, y}));
                vert.normal = XMFLOAT3(0.0, 1.0, 0.0); // TODO: calc normals
                XMStoreFloat4(&vert.color, Colors::Green);
            }
        }
    
        D3D11_SUBRESOURCE_DATA vert_data {};
        vert_data.pSysMem = verts.data();
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
        indices.reserve(num_indices);
        for (int x = 0; x <= width; x++) {
            for (int y = 0; y <= height; y++) {
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
                
                indices[quad_index + 0] = v0;
                indices[quad_index + 1] = v3;
                indices[quad_index + 2] = v1;
                indices[quad_index + 3] = v2;
                indices[quad_index + 4] = v3;
                indices[quad_index + 5] = v0;
            }
        }

        D3D11_SUBRESOURCE_DATA index_data {};
        index_data.pSysMem = indices.data();
        device->CreateBuffer(&bd, &index_data, &ib);
    }

    // TODO: vertex attribute information. how are we storing this in DX11?
}

ID3D11VertexShader* Patch::vs;
ID3D11PixelShader* Patch::ps;

void Patch::setup_patches(ID3D11Device* device) {
    compile_vertex_shader(device, L"terrain.hlsl", "main_vs", &vs);
    compile_pixel_shader(device, L"terrain.hlsl", "main_ps", &ps);
}

inline void Patch::render(ID3D11Device* device) {
    // okay this is going to be a lot more work than DX9. Need to set up skeleton renderer with a camera.
    assert(false);
}

#include "terrain_mesh.h"

#include <vector>
#include <WICTextureLoader.h>

#include "dxutils.h"
#include "renderer/renderer.h"

// Patches

inline HRESULT Patch::create_mesh(Heightmap& hm, RECT src, ID3D11Device* device) {
    int width = src.right - src.left;
    int height = src.bottom - src.top;
    size.x = width;
    size.y = height;
    position.x = src.right;
    position.y = src.top;
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
                vert.pos = XMFLOAT3((float)x, (float)y, hm.get_height({src.left + x, src.top + y}));
                vert.normal = XMFLOAT3(0.0, 1.0, 0.0); // TODO: calc normals
                vert.uv = XMFLOAT2((float)(x + src.left) / (float)hm.size.x, (float)(y + src.top) / (float)hm.size.y);
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
ID3D11Buffer* Patch::view_cb;
ID3D11RasterizerState* Patch::rasterizer_state;

void Patch::setup_patches(ID3D11Device* device) {
    compile_vertex_shader(device, L"terrain.hlsl", "main_vs", &vs, &vs_bytecode);
    compile_pixel_shader(device, L"terrain.hlsl", "main_ps", &ps);
}

inline void Patch::render(RenderState rs) {
    u32 stride = sizeof(TerrainVert);
    u32 offset = 0;
    context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    context->IASetIndexBuffer(ib, DXGI_FORMAT_R16_UINT, 0);

    context->VSSetShader(vs, nullptr, 0);
	{
        D3D11_MAPPED_SUBRESOURCE subresource {};
		XMMATRIX model = DirectX::XMMatrixIdentity();
        model = DirectX::XMMatrixTranslation((float)position.x, (float)position.y, 0);
        XMMATRIX mvp = XMMatrixMultiply(model, XMMatrixMultiply(rs.view, rs.projection));

		//context->UpdateSubresource(view_cb, 0, 0, &mvp, sizeof(XMMATRIX), 0);
		context->Map(view_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
        memcpy(subresource.pData, &mvp, sizeof(mvp));
        context->Unmap(view_cb, 0);
		context->VSSetConstantBuffers(0, 1, &view_cb);
    }

    context->RSSetState(rasterizer_state);



    context->PSSetShader(ps, nullptr, 0);

    context->DrawIndexed(size.x*size.y*6, 0, 0);
}


// Terrain

void Terrain::generate_random_terrain(int seed, int num_patches) {
    heightmap.create_random_heightmap(seed, 1.0, 0.5, 5);
}

void Terrain::create_patches(int num_patches) {
	for (int y = 0; y < num_patches; ++y) {
		for (int x = 0; x < num_patches; ++x) {
            int width = size.x / num_patches;
            int height = size.y / num_patches;
            int org_x = width * x;
            int org_y = height * y;
            RECT rect = {
                org_x,
                org_y,
                org_x + width,
                org_y + height,
            };
            patches.emplace_back();
            patches[x + y*num_patches].create_mesh(heightmap, rect, device);
        }
    }

    D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
        {"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    HRASSERT(device->CreateInputLayout(input_layout_desc, NUM_ELEMENTS(input_layout_desc), Patch::vs_bytecode->GetBufferPointer(), Patch::vs_bytecode->GetBufferSize(), &input_layout));
    assert(input_layout);

    HRASSERT(CreateWICTextureFromFile(device, context, L"../resources/textures/grass.jpg", nullptr, &grass_srv));
    HRASSERT(CreateWICTextureFromFile(device, context, L"../resources/textures/water.jpg", nullptr, &water_srv));
    HRASSERT(CreateWICTextureFromFile(device, context, L"../resources/textures/stone.jpg", nullptr, &stone_srv));

    D3D11_BUFFER_DESC view_cb_desc {};
    view_cb_desc.ByteWidth = sizeof(ViewCB);
    view_cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    view_cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    view_cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    view_cb_desc.MiscFlags = 0;

    HRASSERT(device->CreateBuffer(&view_cb_desc, nullptr, &Patch::view_cb));

    D3D11_RASTERIZER_DESC rs_desc {};
    rs_desc.FillMode = D3D11_FILL_SOLID;
    rs_desc.CullMode = D3D11_CULL_BACK;
    rs_desc.FrontCounterClockwise = true;
    rs_desc.DepthBias = 0;
    rs_desc.DepthBiasClamp = 0;
    rs_desc.SlopeScaledDepthBias = 0;
    rs_desc.DepthClipEnable = false;
    rs_desc.ScissorEnable = false;
    rs_desc.MultisampleEnable = false;
    rs_desc.AntialiasedLineEnable = false;

    device->CreateRasterizerState(&rs_desc, &Patch::rasterizer_state);
}

void Terrain::render(RenderState rs) {
    context->IASetInputLayout(input_layout);
    context->PSSetShaderResources(0, 1, &grass_srv);
    context->PSSetShaderResources(1, 1, &water_srv);
    context->PSSetShaderResources(2, 1, &stone_srv);

        // TODO: fix resource leak
    ID3D11SamplerState* bilinear_sampler;
    D3D11_SAMPLER_DESC sampler_desc {};
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HRASSERT(device->CreateSamplerState(&sampler_desc, &bilinear_sampler));
    context->PSSetSamplers(0, 1, &bilinear_sampler);
    // TODO: separate patches, separate positions
    for (Patch& patch : patches)
		patch.render(rs);
}

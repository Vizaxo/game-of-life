#include "mesh.h"

#include "common.h"
#include "obj.h"
#include "renderer.h"
#include "dxutils.h"
#include "app.h"
#include "shader_library.h"

const wchar_t* mesh_resource_path = RESOURCE_DIR L"models";

ID3D11RasterizerState* Mesh::rasterizer_state;

Mesh::Mesh() {
	vs = &circle_vs;
	ps = &circle_ps;

    D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
        {"SV_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    HRASSERT(device->CreateInputLayout(input_layout_desc, NUM_ELEMENTS(input_layout_desc), circle_vs.vs_bytecode->GetBufferPointer(), circle_vs.vs_bytecode->GetBufferSize(), &input_layout));
    assert(input_layout);
}

void Mesh::static_init() {

    //HRASSERT(device->CreateBuffer(&view_cb_desc, nullptr, &view_cb));

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

    device->CreateRasterizerState(&rs_desc, &Mesh::rasterizer_state);
}

HRESULT Mesh::load(const char* name, MeshData& mesh_data) {
	//char path[1024];
	//snprintf(path, 1024, "%s/%s", mesh_resource_path, name);
	//std::unique_ptr<MeshData> mesh_data = importOBJ("testmesh", path);

	// vertex buffer
	num_vertices = (u32)mesh_data.verts.size();
	{
		D3D11_BUFFER_DESC bd{};
		bd.ByteWidth = sizeof(MeshVert) * num_vertices;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = sizeof(MeshVert);

		D3D11_SUBRESOURCE_DATA vert_data{};
		vert_data.pSysMem = mesh_data.verts.data();
		vert_data.SysMemPitch = sizeof(MeshVert);
		HRASSERT(device->CreateBuffer(&bd, &vert_data, &vb));
	}

	// index buffer
	num_indices = (u32)mesh_data.indices.size();
	{
		D3D11_BUFFER_DESC bd{};
		bd.ByteWidth = sizeof(u32) * num_indices;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = sizeof(u32);

		D3D11_SUBRESOURCE_DATA index_data{};
		index_data.pSysMem = mesh_data.indices.data();
		HRASSERT(device->CreateBuffer(&bd, &index_data, &ib));
	}

	//fclose(fp);
	//free(contents);
	return S_OK;
}

void MeshInstance::render(RenderState rs)
{
    context->IASetInputLayout(mesh->input_layout);
    u32 stride = sizeof(MeshVert);
    u32 offset = 0;
    context->IASetVertexBuffers(0, 1, &mesh->vb, &stride, &offset);
    context->IASetIndexBuffer(mesh->ib, DXGI_FORMAT_R32_UINT, 0);

	mesh->vs->set_shader(context);
	{
        D3D11_MAPPED_SUBRESOURCE subresource {};
		XMMATRIX model = DirectX::XMMatrixIdentity();
        model = 
			DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(scale.x,scale.y,scale.z), 
			DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z), 
			DirectX::XMMatrixTranslation((float)position.x, (float)position.y, 0)));
		ViewCB view_cb_data {};
        view_cb_data.mvp = DirectX::XMMatrixMultiply(model, DirectX::XMMatrixMultiply(rs.view, rs.projection));
		view_cb_data.screen_size = {640,480};
		//u64 ticks_ms = GetTickCount64();
		//float time = (float)ticks_ms / 1000.f;
		view_cb_data.time = time_elapsed;
		view_cb_data.last_click_time = last_click_time;
		view_cb_data.num_clicks_low = score & 0xffffffff;
		view_cb_data.num_clicks_high = (score >> 32) & 0xffffffff;

		circle_vs.set<ViewCB>(context, view_cb_data);
    }

    context->RSSetState(mesh->rasterizer_state);

	mesh->ps->set_shader(context);

    context->DrawIndexed(mesh->num_indices, 0, 0);
}

std::unique_ptr<Mesh> quad_mesh;
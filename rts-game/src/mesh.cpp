#include "mesh.h"

#include "common.h"
#include "obj.h"
#include "renderer/renderer.h"
#include "dxutils.h"

const char* mesh_resource_path = "../resources/models";

ID3D11VertexShader* Mesh::vs;
ID3DBlob* Mesh::vs_bytecode;
ID3D11PixelShader* Mesh::ps;
ID3D11InputLayout* Mesh::input_layout;
ID3D11Buffer* Mesh::view_cb;
ID3D11RasterizerState* Mesh::rasterizer_state;

Mesh::Mesh(const char* name) {
	load(name);
}

void Mesh::static_init() {
    D3D11_BUFFER_DESC view_cb_desc {};
    view_cb_desc.ByteWidth = sizeof(ViewCB);
    view_cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    view_cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    view_cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    view_cb_desc.MiscFlags = 0;

    HRASSERT(device->CreateBuffer(&view_cb_desc, nullptr, &view_cb));

    compile_vertex_shader(device, L"mesh.hlsl", "main_vs", &vs, &vs_bytecode);
    compile_pixel_shader(device, L"mesh.hlsl", "main_ps", &ps);

    D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
        {"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    HRASSERT(device->CreateInputLayout(input_layout_desc, NUM_ELEMENTS(input_layout_desc), Mesh::vs_bytecode->GetBufferPointer(), Mesh::vs_bytecode->GetBufferSize(), &input_layout));
    assert(input_layout);

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

HRESULT Mesh::load(const char* name) {
	char path[1024];
	snprintf(path, 1024, "%s/%s", mesh_resource_path, name);
	/*
	FILE* fp;
	size_t size;
	if (!(fp = fopen(path, "r")))
		debug_print(LogLevel::FATAL, "Could not find model file to load");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	u8* contents = (u8*)malloc(size);
	if (!contents)
		debug_print(LogLevel::FATAL, "Malloc failed");

	if (!size == fread(contents, 1, size, fp))
		debug_print(LogLevel::FATAL, "Did not read enough bytes");
		*/

	std::unique_ptr<MeshData> mesh_data = importOBJ("testmesh", path);

	/*
	ofbx::IScene* scene = ofbx::load(contents, size, 0u);
	if (!scene)
		debug_print(LogLevel::FATAL, "Failed to load scene");

	u32 num_meshes = scene->getMeshCount();
		*/

	// vertex buffer
	num_vertices = (u32)mesh_data->verts.size();
	{
		D3D11_BUFFER_DESC bd{};
		bd.ByteWidth = sizeof(MeshVert) * num_vertices;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = sizeof(MeshVert);

		D3D11_SUBRESOURCE_DATA vert_data{};
		vert_data.pSysMem = mesh_data->verts.data();
		vert_data.SysMemPitch = sizeof(MeshVert);
		HRASSERT(device->CreateBuffer(&bd, &vert_data, &vb));
	}

	// index buffer
	num_indices = (u32)mesh_data->indices.size();
	{
		D3D11_BUFFER_DESC bd{};
		bd.ByteWidth = sizeof(u32) * num_indices;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = sizeof(u32);

		D3D11_SUBRESOURCE_DATA index_data{};
		index_data.pSysMem = mesh_data->indices.data();
		HRASSERT(device->CreateBuffer(&bd, &index_data, &ib));
	}

	//fclose(fp);
	//free(contents);
	return S_OK;
}

void MeshInstance::render(RenderState rs) {
{
    context->IASetInputLayout(mesh->input_layout);
    u32 stride = sizeof(MeshVert);
    u32 offset = 0;
    context->IASetVertexBuffers(0, 1, &mesh->vb, &stride, &offset);
    context->IASetIndexBuffer(mesh->ib, DXGI_FORMAT_R32_UINT, 0);

    context->VSSetShader(mesh->vs, nullptr, 0);
	{
        D3D11_MAPPED_SUBRESOURCE subresource {};
		XMMATRIX model = DirectX::XMMatrixIdentity();
        model = 
			DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(scale.x,scale.y,scale.z), 
			DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z), 
			DirectX::XMMatrixTranslation((float)position.x, (float)position.y, 0)));
        XMMATRIX mvp = DirectX::XMMatrixMultiply(model, DirectX::XMMatrixMultiply(rs.view, rs.projection));

		//context->UpdateSubresource(view_cb, 0, 0, &mvp, sizeof(XMMATRIX), 0);
		HRASSERT(context->Map(mesh->view_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));
        memcpy(subresource.pData, &mvp, sizeof(mvp));
        context->Unmap(mesh->view_cb, 0);
		context->VSSetConstantBuffers(0, 1, &mesh->view_cb);
    }

    context->RSSetState(mesh->rasterizer_state);

    context->PSSetShader(mesh->ps, nullptr, 0);

    context->DrawIndexed(mesh->num_indices, 0, 0);
}

}
#include "renderer.h"

#include <wrl/wrappers/corewrappers.h>
#include <WICTextureLoader.h>

#include "app.h"
#include "render_text.h"
#include "mesh.h"
#include "game.h"

#include "shader.h"
#include "shader_library.h"
#include "constant_buffer.h"

#include "simulation.h"

ID3D11Device* device;
ID3D11DeviceContext* context;
IDXGISwapChain* swapchain;
ID3D11RenderTargetView* backbuffer_rtv;
ID3D11Buffer* cell_buffer;
ID3D11ShaderResourceView* cells_srv;

Microsoft::WRL::Wrappers::RoInitializeWrapper* initialize;

std::unique_ptr<MeshInstance> screen_quad;

std::unique_ptr<ConstantBuffer<ViewCB>> view_cb;
std::unique_ptr<ConstantBuffer<SimState>> sim_state_cb;

ID3D11RenderTargetView* get_backbuffer_rtv() {
	DXGI_SWAP_CHAIN_DESC sc_desc;
	swapchain->GetDesc(&sc_desc);
	ID3D11Texture2D* backbuffer;
	HRASSERT(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer));

	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc {};
	rtv_desc.Format = sc_desc.BufferDesc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView* rtv;
	HRASSERT(device->CreateRenderTargetView(backbuffer, &rtv_desc, &rtv));
	assert(rtv);
	return rtv;
}

void renderer_init(App &app) {
	device = app.device;
	context = app.context;
	swapchain = app.swapchain;

	load_font();

	// Initialise Windows Imaging Component for DirectXTK texture loading
#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
    initialize = new Microsoft::WRL::Wrappers::RoInitializeWrapper(RO_INIT_MULTITHREADED);
    if (FAILED(*initialize))
		debug_print(LogLevel::FATAL, "Could not initialise WIC");
#else
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
		debug_print(LogLevel::FATAL, "Could not initialise WIC");
#endif

	backbuffer_rtv = get_backbuffer_rtv();

	init_shader_library(device);

	Mesh::static_init();
	quad_mesh = std::make_unique<Mesh>(visualise_vs, visualise_ps);
	quad_mesh->load("quad", quad_mesh_data);
	 
	screen_quad = std::make_unique<MeshInstance>(quad_mesh.get(), XMFLOAT3(0., 0., 0.));
	
	view_cb = std::make_unique<ConstantBuffer<ViewCB>>(create_constant_buffer<ViewCB>(device));
	sim_state_cb = std::make_unique<ConstantBuffer<SimState>>(create_constant_buffer<SimState>(device));

	// structured buffer
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(cells);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(CellData);
		HRASSERT(device->CreateBuffer(&desc, nullptr, &cell_buffer));

		HRASSERT(device->CreateShaderResourceView(cell_buffer, nullptr, &cells_srv));
	}
}

void set_viewport() {
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 640;
	viewport.Height = 480;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);
}

XMMATRIX setup_camera() {
	//XMVECTOR cam_pos = DirectX::XMVectorSet(70.f, 50.f, 50.f, 1.f);
	//XMVECTOR look_at = DirectX::XMVectorSet(71.f, 50.f, 0.f, 1.f);
	XMVECTOR cam_pos = DirectX::XMVectorSet(0.f, 0.f, 1000.f, 1.f);
	XMVECTOR look_at = DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMVECTOR cam_dir = DirectX::XMVectorSubtract(look_at, cam_pos);

	XMVECTOR world_up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 1.f);
	XMVECTOR cam_right = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(cam_dir, world_up));
	XMVECTOR cam_up = DirectX::XMVector3Cross(cam_right, cam_dir);

	return XMMatrixLookAtLH(cam_pos, look_at, world_up);
}

void set_view_cb(ID3D11DeviceContext* context, RenderState& rs) {
	XMMATRIX model = DirectX::XMMatrixIdentity();
	/*
	TODO: model matrix?
	model =
		DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(scale.x,scale.y,scale.z),
		DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z),
		DirectX::XMMatrixTranslation((float)position.x, (float)position.y, 0)));
		*/
	ViewCB view_cb_data{};
	view_cb_data.mvp = DirectX::XMMatrixMultiply(model, DirectX::XMMatrixMultiply(rs.view, rs.projection));
	view_cb_data.screen_size = { 640,480 };
	view_cb_data.time = time_elapsed;

	set_cb(context, *view_cb, &view_cb_data);
}

void set_cells_buffer(ID3D11DeviceContext* context) {
	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRASSERT(context->Map(cell_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));
	memcpy(subresource.pData, cells, sizeof(cells));
	context->Unmap(cell_buffer, 0);
	context->PSSetShaderResources(0, 1, &cells_srv);
}

HRESULT render(App& app) {
	RenderState rs;
	rs.view = setup_camera();
	rs.projection = XMMatrixIdentity(); //XMMatrixPerspectiveFovLH(80, 4.f/3.f, 0.1f, 1000.f);
	rs.projection = XMMatrixOrthographicLH(640, 480, -10, 10);

	context->ClearRenderTargetView(backbuffer_rtv, Colors::Aqua);

	set_viewport();
	context->OMSetRenderTargets(1, &backbuffer_rtv, nullptr);
	// set depth stencil state

	set_view_cb(context, rs);
	bind_cb(context, *view_cb, 0);

	set_cb(context, *sim_state_cb, &sim_state);
	bind_cb(context, *sim_state_cb, 1);

	set_cells_buffer(context);

	render_mesh(*screen_quad, rs);

	swapchain->Present(0, 0);

	return S_OK;
}
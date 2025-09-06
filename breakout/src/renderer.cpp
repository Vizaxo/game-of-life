#include "renderer.h"

#include <wrl/wrappers/corewrappers.h>
#include <WICTextureLoader.h>

#include "app.h"
#include "render_text.h"
#include "mesh.h"
#include "game.h"

ID3D11Device* device;
ID3D11DeviceContext* context;
IDXGISwapChain* swapchain;
ID3D11RenderTargetView* backbuffer_rtv;

Microsoft::WRL::Wrappers::RoInitializeWrapper* initialize;

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

	Mesh::static_init();
	quad_mesh = std::make_unique<Mesh>();
	quad_mesh->load("quad", quad_mesh_data);
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

HRESULT render(App& app) {
	RenderState rs;
	rs.view = setup_camera();
	rs.projection = XMMatrixIdentity(); //XMMatrixPerspectiveFovLH(80, 4.f/3.f, 0.1f, 1000.f);
	rs.projection = XMMatrixOrthographicLH(640, 480, -10, 10);

	context->ClearRenderTargetView(backbuffer_rtv, Colors::Aqua);

	set_viewport();
	context->OMSetRenderTargets(1, &backbuffer_rtv, nullptr);
	// set depth stencil state

	wchar_t buf[1024];
	swprintf(buf, 1024, L"%lld", score);
	draw_text(buf, Colors::Red);

	for (Block& block : blocks) {
		//block.mesh_instance.set_shader_parameter(SP::Color, block.color);
		block.mesh_instance.render(rs);
	}

	swapchain->Present(0, 0);

	return S_OK;
}
#include "renderer.h"

#include <wrl/wrappers/corewrappers.h>
#include <WICTextureLoader.h>

#include "app.h"
#include "render_text.h"
#include "terrain_mesh.h"

ID3D11Device* device;
ID3D11DeviceContext* context;
IDXGISwapChain* swapchain;
ID3D11RenderTargetView* backbuffer_rtv;
Terrain terrain({100, 100});

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
	Patch::setup_patches(device);
	terrain.generate_random_terrain(550465, 1);
	terrain.create_patches(5);

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

HRESULT render() {
	ID3D11RenderTargetView* backbuffer_rtv = get_backbuffer_rtv();

	context->ClearRenderTargetView(backbuffer_rtv, Colors::Aqua);

	set_viewport();
	context->OMSetRenderTargets(1, &backbuffer_rtv, nullptr);
	// set depth stencil state

	draw_debug_text(L"Hello, world");
	terrain.render();

	swapchain->Present(0, 0);

	return S_OK;
}
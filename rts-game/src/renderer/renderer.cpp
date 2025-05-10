#include "renderer.h"

#include "app.h"

ID3D11Device* device;
ID3D11DeviceContext* context;
IDXGISwapChain* swapchain;

ID3D11RenderTargetView* get_backbuffer_rtv() {
	DXGI_SWAP_CHAIN_DESC sc_desc;
	swapchain->GetDesc(&sc_desc);
	ID3D11Texture2D* backbuffer;
	HRASSERT(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer));

	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc {};
	rtv_desc.Format = sc_desc.BufferDesc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView* backbuffer_rtv;
	HRASSERT(device->CreateRenderTargetView(backbuffer, &rtv_desc, &backbuffer_rtv));
	assert(backbuffer_rtv);
	return backbuffer_rtv;
}

HRESULT render(App &app) {
	device = app.device;
	context = app.context;
	swapchain = app.swapchain;

	ID3D11RenderTargetView* backbuffer_rtv = get_backbuffer_rtv();

	context->ClearRenderTargetView(backbuffer_rtv, Colors::Aqua);

	swapchain->Present(0, 0);

	return S_OK;
}
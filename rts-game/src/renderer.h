#pragma once

#include "common.h"
#include "app.h"

extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern IDXGISwapChain* swapchain;

struct ViewCB {
    XMMATRIX mvp;
};

struct RenderState {
    XMMATRIX view;
    XMMATRIX projection;
};


HRESULT render();
void renderer_init(App &app);

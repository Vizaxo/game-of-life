#pragma once

#include "common.h"
#include "app.h"

extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern IDXGISwapChain* swapchain;

HRESULT render();
void renderer_init(App &app);

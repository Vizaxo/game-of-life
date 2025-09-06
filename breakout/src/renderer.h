#pragma once

#include "common.h"
#include "app.h"

extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern IDXGISwapChain* swapchain;

struct ViewCB {
    XMMATRIX mvp;
    XMFLOAT2 screen_size;
    float time;
    float last_click_time;
    u32 num_clicks_low;
    u32 num_clicks_high;
};

struct RenderState {
    XMMATRIX view;
    XMMATRIX projection;
};


HRESULT render(App& app);
void renderer_init(App &app);

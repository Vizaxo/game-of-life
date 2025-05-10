#pragma once

#include "common.h"

struct App {
    HRESULT init(HINSTANCE hInstance, int width, int height);
    HRESULT update(float dt);
    HRESULT render();
    HRESULT cleanup();
    HRESULT quit();

    const LPCWCHAR class_name = TEXT("D3DWND");

    void register_window_class(HINSTANCE hInstance) const;
    void create_window(HINSTANCE hInstance, int width, int height);
    HRESULT create_device_and_swapchain(int width, int height);

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapchain = nullptr;
    HWND hwnd;
};
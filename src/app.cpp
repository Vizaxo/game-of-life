#include "app.h"

HRESULT App::init(HINSTANCE hInstance, int width, int height) {
    WNDCLASS wc {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)::DefWindowProcW;
    wc.hInstance = hInstance;
    wc.lpszClassName = class_name;

    RegisterClass(&wc);

    hwnd = CreateWindow(class_name, TEXT("RTS Game"), WS_EX_TOPMOST, 0, 0, width, height, 0, 0, hInstance, NULL);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    u32 create_flags = 0;
#if _DEBUG
    create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
    swapchain_desc.BufferDesc.Width = width;
    swapchain_desc.BufferDesc.Height = height;
    swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.SampleDesc.Quality = 0;
    swapchain_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.BufferCount = 2;
    swapchain_desc.OutputWindow = hwnd;
    swapchain_desc.Windowed = true;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    IDXGISwapChain* swapchain;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_flags, NULL, 0, D3D11_SDK_VERSION, &swapchain_desc, &swapchain, &device, NULL, &device_context);
    if (FAILED(hr))
        return 0;
}

HRESULT App::update(float dt) {
    return 0;
}

HRESULT App::render() {
    return 0;
    
}

HRESULT App::cleanup() {
    return 0;
    
}

HRESULT App::quit() {
    return 0;
    
}

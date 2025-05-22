#include "app.h"

#include "renderer.h"
#include "dxutils.h"
#include "mesh.h"

App* app;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_LBUTTONDOWN:
        app->left_mouse_clicked(lParam & 0xff, (lParam >> 16) & 0xff);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void App::register_window_class(HINSTANCE hInstance) {
    app = this;
    WNDCLASS wc {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = class_name;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClass(&wc);
}

void App::create_window(HINSTANCE hInstance, int width, int height) {
    hwnd = CreateWindow(class_name, TEXT("Circle clicker"), WS_SYSMENU, 0, 0, width, height, 0, 0, hInstance, NULL);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

HRESULT App::create_device_and_swapchain(int width, int height) {
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

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_flags, nullptr, 0, D3D11_SDK_VERSION, &swapchain_desc, &swapchain, &device, nullptr, &context);
    return hr;
}

HRESULT App::init(HINSTANCE hInstance, int width, int height) {
    register_window_class(hInstance);
    
    create_window(hInstance, width, height);

    HRESULT hr = create_device_and_swapchain(width, height);
    if (FAILED(hr))
        return hr;

    renderer_init(*this);

    //Mesh train_mesh("train-diesel-a.fbx");

    return S_OK;
}

HRESULT App::update(double dt, double t) {
    time_elapsed = t;
    return 0;
}

HRESULT App::render() {
    return ::render(*this);
}

HRESULT App::cleanup() {
    return 0;
}

HRESULT App::quit() {
    return 0;
}

void App::left_mouse_clicked(u16 x, u16 y) {
    ++score;
}

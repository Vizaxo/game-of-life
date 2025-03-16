#include <Windows.h>
#include <D3D11.h>
#include "common.h"

struct App {
    HRESULT init(HINSTANCE hInstance, int width, int height);
    HRESULT update(float dt);
    HRESULT render();
    HRESULT cleanup();
    HRESULT quit();

    const LPCWCHAR class_name = TEXT("D3DWND");
private:
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* device_context = nullptr;
    HWND hwnd;
};
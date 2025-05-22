#include <D3D11.h>
#include <d3dcompiler.h>
#include "common.h"

#ifdef _TEST
inline const wchar_t* SHADER_DIR = L"../rts-test/test/shaders";
#else
inline const wchar_t* SHADER_DIR = L"../rts-game/src/shaders"; // TODO: properly set this for packaged builds
#endif

enum class ShaderType {
    Vertex,
    Pixel,
};

inline HRESULT compile_shader(LPCWSTR src_file, LPCSTR entrypoint, ShaderType type, ID3DBlob** shader_blob) {
    LPCSTR target_str;
    switch (type) {
    case ShaderType::Vertex:  target_str = "vs_5_0"; break;
    case ShaderType::Pixel:   target_str = "ps_5_0"; break;
    default:                  target_str = "UNKNOWN"; break;
    }
    u32 compile_flags = 0;
#if _DEBUG
    // TODO: make this configurable, not just with an #ifdef
    compile_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* error_blob = nullptr;
    wchar_t shader_path[MAX_PATH];
    int ret = swprintf(shader_path, MAX_PATH, L"%s/%s", SHADER_DIR, src_file);
    if (ret < 0 || ret >= MAX_PATH)
        return E_FAIL;
    
recompile:
    HRESULT hr = D3DCompileFromFile(shader_path, nullptr, nullptr, entrypoint, target_str, compile_flags, 0, shader_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
#if _DEBUG
            debug_print(LogLevel::PRINT, (LPCSTR)error_blob->GetBufferPointer());
            DebugBreak();
            error_blob->Release();
            goto recompile;
#else
            debug_print(LogLevel::FATAL, (LPCSTR)error_blob->GetBufferPointer());
            error_blob->Release();
#endif
        } else {
            switch (hr) {
            case 0x80070002:
                debug_print(LogLevel::PRINT, "Shader file not found");
                goto print_dir_info;
            case 0x80070003:
                debug_print(LogLevel::PRINT, "Shader directory not found");
            print_dir_info:
                debug_print(LogLevel::PRINT, "Current dir:");
                wchar_t dir[MAX_PATH];
                GetCurrentDirectory(MAX_PATH, dir);
                debug_print(LogLevel::PRINT, dir);
                debug_print(LogLevel::PRINT, shader_path);
#if _DEBUG
                DebugBreak();
                goto recompile;
#else
                debug_print(LogLevel::FATAL, "Failed to compile shader");
#endif
                break;
            default: debug_print(LogLevel::FATAL, "Undefined error compiling shader\n"); break;
            }
        }
        return hr;
    }
    return S_OK;
}

inline HRESULT compile_pixel_shader(ID3D11Device* device, LPCWSTR src_file, LPCSTR entrypoint, ID3D11PixelShader** ps) {
    ID3DBlob* shader_blob;
    HRESULT hr = compile_shader(src_file, entrypoint, ShaderType::Pixel, &shader_blob);
    if (!FAILED(hr))
        hr = device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, ps);
    return hr;
}

inline HRESULT compile_vertex_shader(ID3D11Device* device, LPCWSTR src_file, LPCSTR entrypoint, ID3D11VertexShader** vs, ID3DBlob** bytecode = nullptr) {
    ID3DBlob* shader_blob;
    HRESULT hr = compile_shader(src_file, entrypoint, ShaderType::Vertex, &shader_blob);
    if (!FAILED(hr))
        hr = device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, vs);
    *bytecode = shader_blob;
    return hr;
}

#define NUM_ELEMENTS(arr) (sizeof(arr)/sizeof(arr[0]))

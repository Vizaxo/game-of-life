#include <d3dcompiler.h>

#include "types.h"

enum ShaderType {
    Vertex,
    Pixel,
}; 

HRESULT compile_shader(LPCWSTR src_file, LPCSTR entrypoint, ShaderType type, ID3D11Blob* blob_out) {
    LPCSTR target_str;
    switch (type) {
    case Vertex:
        target_str = "vs_5_0";
        break;
    case Pixel:
        target_str = "ps_5_0";
        break;
    default:
        target_str = "UNKNOWN ??";
        break;
    }
    u32 compile_flags = 0;
#if _DEBUG
    // TODO: make this configurable, not just with an #ifdef
    compile_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* error_blob = nullptr;
    ID3DBlob* shader_blob = nullptr;
    HRESULT hr = D3DCompileFromFile(src_file, nullptr, nullptr, entrypoint, target_str, compile_flags, 0, shader_blob, error_blob);
    if (FAILED(hr)) {
        OutputDebugString((LPCWSTR)error_blob->GetBufferPointer());
        return hr;
    } else {
        blob_out = shader_blob;
        return hr;
    }
}
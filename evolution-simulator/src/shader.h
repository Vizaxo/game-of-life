#pragma once

#include "common.h"
#include "dxutils.h"

// TODO: this means that CBs are tied to the shader. different set of CBs means different shader
// needs to be used. that isn't ideal.

enum class ShaderStage {
    None,
    Vertex,
    Pixel,
    NUM,
};

struct Shader {
    ShaderStage stage = ShaderStage::None;
    LPCWSTR filename;
    LPCSTR entrypoint;
    union {
		ID3D11VertexShader* vs;
        ID3D11PixelShader* ps;
    };
    ID3DBlob* vs_bytecode;
};

Shader compile_vs(ID3D11Device* device, LPCWSTR filename, LPCSTR entrypoint);

Shader compile_ps(ID3D11Device* device, LPCWSTR filename, LPCSTR entrypoint);

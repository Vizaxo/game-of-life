#pragma once

#include "shader.h"
#include "renderer.h"


inline const wchar_t circle_hlsl[] = L"circle.hlsl";

inline const char main_vs[] = "main_vs";
inline const char main_ps[] = "main_ps";

extern ShaderC<ShaderStage::Vertex, circle_hlsl, main_vs, ViewCB> circle_vs;
extern ShaderC<ShaderStage::Pixel, circle_hlsl, main_ps, ViewCB> circle_ps;

static inline HRESULT init_shader_library(ID3D11Device* device) {
    HRASSERT(circle_vs.init(device));
    HRASSERT(circle_ps.init(device));
    return S_OK;
}

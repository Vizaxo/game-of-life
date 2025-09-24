#include "shader_library.h"

Shader visualise_ps;
Shader visualise_vs;

HRESULT init_shader_library(ID3D11Device* device) {
	visualise_vs = compile_vs(device, L"visualise.hlsl", "main_vs");
	visualise_ps = compile_ps(device, L"visualise.hlsl", "main_ps");
	return S_OK;
}

#pragma once

#include "shader.h"
#include "renderer.h"

extern Shader visualise_ps;
extern Shader visualise_vs;

HRESULT init_shader_library(ID3D11Device* device);

#include "shader_library.h"

ShaderC<ShaderStage::Vertex, circle_hlsl, main_vs, ViewCB> circle_vs;
ShaderC<ShaderStage::Pixel, circle_hlsl, main_ps, ViewCB> circle_ps;

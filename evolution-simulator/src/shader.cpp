#include "shader.h"

Shader compile_vs(ID3D11Device* device, LPCWSTR filename, LPCSTR entrypoint) {
    Shader vs;
    vs.stage = ShaderStage::Vertex;
    vs.filename = filename;
    vs.entrypoint = entrypoint;
    HRASSERT(compile_vertex_shader(device, filename, entrypoint, &vs.vs, &vs.vs_bytecode));
    return vs;
}

Shader compile_ps(ID3D11Device* device, LPCWSTR filename, LPCSTR entrypoint) {
    Shader ps;
    ps.stage = ShaderStage::Pixel;
    ps.filename = filename;
    ps.entrypoint = entrypoint;
    HRASSERT(compile_pixel_shader(device, filename, entrypoint, &ps.ps));
    return ps;
}
#pragma once

#include "common.h"
#include "dxutils.h"

// TODO: this means that CBs are tied to the shader. different set of CBs means different shader
// needs to be used. that isn't ideal.

enum class ShaderStage {
    Vertex,
    Pixel,
    NUM,
};

struct Shader {
    virtual HRESULT init(ID3D11Device* device) = 0;
    virtual void set_shader(ID3D11DeviceContext* context) = 0;
};

template <ShaderStage stage, LPCWSTR filename, LPCSTR entrypoint>
struct ShaderB : Shader {};

template <LPCWSTR filename, LPCSTR entrypoint>
struct ShaderB<ShaderStage::Vertex, filename, entrypoint> : Shader {
    ID3D11VertexShader* vs;
    ID3DBlob* vs_bytecode;

    virtual HRESULT init(ID3D11Device* device) {
        return compile(device);
    }

    virtual HRESULT compile(ID3D11Device* device) {
        assert(vs == nullptr);
        return compile_vertex_shader(device, filename, entrypoint, &vs, &vs_bytecode);
    }

    virtual void set_shader(ID3D11DeviceContext* context) {
        context->VSSetShader(vs, nullptr, 0);
    }
};

template <LPCWSTR filename, LPCSTR entrypoint>
struct ShaderB<ShaderStage::Pixel, filename, entrypoint> : Shader {
    ID3D11PixelShader* ps;

    virtual HRESULT init(ID3D11Device* device) {
        return compile(device);
    }

    virtual HRESULT compile(ID3D11Device* device) {
        assert(ps == nullptr);
        return compile_pixel_shader(device, filename, entrypoint, &ps);
    }

    virtual void set_shader(ID3D11DeviceContext* context) {
        context->PSSetShader(ps, nullptr, 0);
    }
};

template <typename... CBs>
struct ConstantBuffers;

template <>
struct ConstantBuffers<> {
    HRESULT init(ID3D11Device* device) {
        return S_OK;
    }
};

template <typename CB, typename... CBs>
struct ConstantBuffers<CB, CBs...> : ConstantBuffers<CBs...> {
    CB here;
    ID3D11Buffer* cb;
    template <typename T> T& get() { return ConstantBuffers<CBs>::get(); }
    template <> CB& get<CB>() {return here;}

    HRESULT init(ID3D11Device* device) {
		D3D11_BUFFER_DESC view_cb_desc{};
		view_cb_desc.ByteWidth = sizeof(CB);
		view_cb_desc.Usage = D3D11_USAGE_DYNAMIC;
		view_cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		view_cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		view_cb_desc.MiscFlags = 0;

		HRASSERT(device->CreateBuffer(&view_cb_desc, nullptr, &cb));

        HRASSERT(ConstantBuffers<CBs...>::init(device));

        return S_OK;
    }

    template <typename T> HRESULT set(ID3D11DeviceContext* context, T& data) { return ConstantBuffers<CBs>::set(context, data); }
    template <> HRESULT set<CB>(ID3D11DeviceContext* context, CB& data) {
        D3D11_MAPPED_SUBRESOURCE subresource {};
		HRASSERT(context->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));
        memcpy(subresource.pData, &data, sizeof(CB));
        context->Unmap(cb, 0);
		context->VSSetConstantBuffers(0, 1, &cb);
		context->PSSetConstantBuffers(0, 1, &cb);

        return S_OK;
    }
};

template <ShaderStage stage, LPCWSTR filename, LPCSTR entrypoint, typename... CBs>
struct ShaderC : ShaderB<stage, filename, entrypoint> {
    ConstantBuffers<CBs...> cbs;
    template <typename T> T& get() { return cbs.get<T>(); }
    template <typename T> HRESULT set(ID3D11DeviceContext* context, T& data) { return cbs.set<T>(context, data); }

    virtual HRESULT init(ID3D11Device* device) {
        ShaderB<stage, filename, entrypoint>::init(device);
        HRASSERT(cbs.init(device));
        return S_OK;
    }
};

static inline wchar_t filename[] = L"test.hlsl";
static inline char entrypoint[] = "test";
struct TestCB {
    float a;
    float b;
};

static inline void test() {
    ID3D11Device* device;
    ShaderC<ShaderStage::Pixel, filename, entrypoint, TestCB> shader;
    shader.init(device);
    shader.get<TestCB>().a = 10.0;
}

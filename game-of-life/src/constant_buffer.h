#pragma once

#include "common.h"

template<typename T>
struct ConstantBuffer {
	ID3D11Buffer* cb;
	void set_data(T& t);
};


template <typename T>
ConstantBuffer<T> create_constant_buffer(ID3D11Device* device) {
	ConstantBuffer<T> ret;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(T);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	HRASSERT(device->CreateBuffer(&desc, nullptr, &ret.cb));

	return ret;
}

template <typename T>
void set_cb(ID3D11DeviceContext* context, ConstantBuffer<T>& cb, T* data) {
	D3D11_MAPPED_SUBRESOURCE subresource{};
	HRASSERT(context->Map(cb.cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));

	memcpy(subresource.pData, (void*)data, sizeof(T));

	context->Unmap(cb.cb, 0);
}

template <typename T>
void bind_cb(ID3D11DeviceContext* context, ConstantBuffer<T>& cb, u32 slot) {
	context->VSSetConstantBuffers(slot, 1, &cb.cb);
	context->PSSetConstantBuffers(slot, 1, &cb.cb);
}

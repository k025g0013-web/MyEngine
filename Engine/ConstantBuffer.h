#pragma once
#include <wrl.h>

#include <d3d12.h>
#include <cstdint>

class ConstantBuffer {
public:
    void Initialize(
        ID3D12Device *device, size_t sizeInBytes
    );

    void *Map();

    // getter
    ID3D12Resource *GetResource() const { return resource_.Get(); }
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
        return resource_->GetGPUVirtualAddress();
    }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    void *mappedData_ = nullptr;
};
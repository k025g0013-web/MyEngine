#pragma once
#include <wrl.h>

#include <d3d12.h>
#include <cstdint>

class IndexBuffer {
public:
    void Initialize(
        ID3D12Device *device, size_t sizeInBytes
    );

    void *Map();

    // getter
    ID3D12Resource *GetResource() const { return resource_.Get(); }
    const D3D12_INDEX_BUFFER_VIEW& GetView() const {
        return indexBufferView_;
    }

private:
    Microsoft::WRL::ComPtr <ID3D12Resource> resource_;

    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    void *mappedData_ = nullptr;
};
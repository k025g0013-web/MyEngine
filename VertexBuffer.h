#pragma once
#include <wrl.h>

#include <d3d12.h>
#include <cstdint>

class VertexBuffer {
public:
    void Initialize(
        ID3D12Device *device, size_t sizeInBytes, uint32_t strideInBytes
    );

    void *Map();

    // getter
    ID3D12Resource *GetResource() const { return resource_.Get(); }
    const D3D12_VERTEX_BUFFER_VIEW &GetView() const {
        return vertexBufferView_;
    }

private:
    Microsoft::WRL::ComPtr <ID3D12Resource> resource_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    void *mappedData_ = nullptr;
};
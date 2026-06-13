#pragma once

#pragma comment(lib, "d3d12.lib")

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

#include "Math/Vector.h"

// 頂点データ
struct VertexData {
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

class MeshBuffer {
public:
    ~MeshBuffer();  // デストラクタで自動 Unmap するように安全性を向上

    // VertexBuffer初期化
    void InitializeAsVertex(ID3D12Device *device, size_t sizeInBytes, uint32_t strideInBytes);

    // IndexBuffer初期化
    void InitializeAsIndex(ID3D12Device *device, size_t sizeInBytes);

    // ConstantBuffer初期化
    void InitializeAsConstant(ID3D12Device *device, size_t sizeInBytes);

    // データの書き込み
    void *Map();
    void Unmap();

    // === getter ===
    ID3D12Resource *GetResource() const { return resource_.Get(); }

    // VertexBuffer
    const D3D12_VERTEX_BUFFER_VIEW *GetVertexBufferView() const { return &vertexBufferView_; }

    // IndexBuffer
    const D3D12_INDEX_BUFFER_VIEW *GetIndexBufferView() const { return &indexBufferView_; }
    
    // ConstantBuffer
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    void *mappedData_ = nullptr;

    // 各種BufferView
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
};
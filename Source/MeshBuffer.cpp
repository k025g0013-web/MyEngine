#include "MeshBuffer.h"
#include "Utils/ResourceHelper.h"
#include <cassert>

MeshBuffer::~MeshBuffer() {
    Unmap();
}

void *MeshBuffer::Map() {
    if (!mappedData_) {
        HRESULT hr = resource_->Map(0, nullptr, &mappedData_);
        assert(SUCCEEDED(hr));
        (void)hr;
    }
    return mappedData_;
}

void MeshBuffer::Unmap() {
    if (mappedData_) {
        resource_->Unmap(0, nullptr);
        mappedData_ = nullptr;
    }
}

// VertexBuffer初期化
void MeshBuffer::InitializeAsVertex(ID3D12Device *device, size_t sizeInBytes, uint32_t strideInBytes) {
    // Resource生成
    resource_ = CreateBufferResource(device, sizeInBytes);

    // VBV生成
    vertexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeInBytes);
    vertexBufferView_.StrideInBytes = strideInBytes;
}

// IndexBuffer初期化
void MeshBuffer::InitializeAsIndex(ID3D12Device *device, size_t sizeInBytes) {
    // Resource生成
    resource_ = CreateBufferResource(device, sizeInBytes);

    // IBV生成
    indexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = static_cast<UINT>(sizeInBytes);
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT; // インデックスはuint32_t固定
}

// ConstantBuffer初期化
void MeshBuffer::InitializeAsConstant(ID3D12Device *device, size_t sizeInBytes) {
    // 定数バッファは256バイトの倍数である必要があるため補正
    size_t alignmentSize = (sizeInBytes + 255) & ~255;
    resource_ = CreateBufferResource(device, alignmentSize);
}
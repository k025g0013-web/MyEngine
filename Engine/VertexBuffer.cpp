#include "VertexBuffer.h"

#include "ResourceUtils.h"	// ResourceUtils

void VertexBuffer::Initialize(
    ID3D12Device *device, size_t sizeInBytes, uint32_t strideInBytes
) {
    // Resource生成
    resource_ = CreateBufferResource(device, sizeInBytes);

    // VBV生成
    vertexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeInBytes);
    vertexBufferView_.StrideInBytes = strideInBytes;
}

void *VertexBuffer::Map() {
    resource_->Map(0, nullptr, &mappedData_);
    return mappedData_;
}
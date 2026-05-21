#include "ConstantBuffer.h"

#include "ResourceUtils.h"  // ResourceUtils

void ConstantBuffer::Initialize(
    ID3D12Device *device, size_t sizeInBytes
) {
    // Resource生成
    resource_ = CreateBufferResource(device, sizeInBytes);
}

void *ConstantBuffer::Map() {
    resource_->Map(0, nullptr, &mappedData_);
    return mappedData_;
}
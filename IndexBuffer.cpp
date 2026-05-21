#include "IndexBuffer.h"

#include "ResourceUtils.h"	// ResourceUtils

void IndexBuffer::Initialize(
    ID3D12Device *device, size_t sizeInBytes
) {
    resource_ = CreateBufferResource(device, sizeInBytes);
    
    // リソースの先頭のアドレスから使う
    indexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    // 使用するリソースのサイズはインデックスから6つ分のサイズ
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
    // インデックスはuint32_tとする
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void *IndexBuffer::Map() {
    resource_->Map(0, nullptr, &mappedData_);
    return mappedData_;
}
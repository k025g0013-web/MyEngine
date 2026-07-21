#include "MeshBuffer.h"
#include "Utils/ResourceHelper.h"
#include <cassert>

namespace Kizuna {
    MeshBuffer::~MeshBuffer() {
        // マッピングを解除する
        Unmap();
    }

    void *MeshBuffer::Map() {
        // 未マッピングの場合のみマッピングする
        if (!mappedData_) {
            HRESULT hr = resource_->Map(0, nullptr, &mappedData_);
            assert(SUCCEEDED(hr));
            (void)hr;
        }

        return mappedData_;
    }

    void MeshBuffer::Unmap() {
        // マッピング済みであれば解除する
        if (mappedData_) {
            resource_->Unmap(0, nullptr);
            mappedData_ = nullptr;
        }
    }

    // VertexBuffer初期化
    void MeshBuffer::InitializeAsVertex(ID3D12Device *device, size_t sizeInBytes, uint32_t strideInBytes) {
        // VertexBuffer用リソースを生成する
        resource_ = CreateBufferResource(device, sizeInBytes);

        // VertexBufferViewを設定する
        vertexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
        vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeInBytes);
        vertexBufferView_.StrideInBytes = strideInBytes;
    }

    // IndexBuffer初期化
    void MeshBuffer::InitializeAsIndex(ID3D12Device *device, size_t sizeInBytes) {
        // IndexBuffer用リソースを生成する
        resource_ = CreateBufferResource(device, sizeInBytes);

        // IndexBufferViewを設定する
        indexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
        indexBufferView_.SizeInBytes = static_cast<UINT>(sizeInBytes);
        indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
    }

    // ConstantBuffer初期化
    void MeshBuffer::InitializeAsConstant(ID3D12Device *device, size_t sizeInBytes) {
        // ConstantBufferは256バイト境界へ揃える
        size_t alignmentSize = (sizeInBytes + 255) & ~255;

        // ConstantBuffer用リソースを生成する
        resource_ = CreateBufferResource(device, alignmentSize);
    }
}
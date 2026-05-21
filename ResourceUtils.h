#pragma once
#include <d3d12.h>
#include <cassert>

// Resource作成
inline ID3D12Resource *CreateBufferResource(ID3D12Device *device, size_t sizeInBytes) {
    // 頂点リソース用のヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    // 頂点リソースの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    // バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes;
    // バッファの場合はこれらは1にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    // 実際に頂点リソース
    ID3D12Resource *resource = nullptr;
    HRESULT hr =
        device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
            &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));
    (void)hr;

    return resource;
}
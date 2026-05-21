#include "DepthStencil.h"

#include <cassert>

void DepthStencil::Initialize(ID3D12Device *device, uint32_t width, uint32_t height) {
    // DSV Heap生成
    dsvHeap_.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

    // DepthStencilResource生成
    CreateDepthStencilResource(device, width, height);

    // DSV生成
    CreateDepthStencilView(device);
}

void DepthStencil::CreateDepthStencilResource(ID3D12Device *device, uint32_t width, uint32_t height) {
    // Resource設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.MipLevels = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // Heap設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // Clear設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthClearValue.DepthStencil.Depth = 1.0f;

    // Resource生成
    HRESULT hr =
        device->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, 
            D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClearValue,
            IID_PPV_ARGS(depthStencilResource_.GetAddressOf())
        );

    assert(SUCCEEDED(hr));
}

void DepthStencil::CreateDepthStencilView(ID3D12Device *device) {
    dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    device->CreateDepthStencilView(
        depthStencilResource_.Get(), &dsvDesc_, dsvHeap_.GetCPUDescriptorHandle(0)
    );
}
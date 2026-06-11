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
    resourceDesc.Width = width;		// Textureの幅
    resourceDesc.Height = height;	// Textureの高さ
    resourceDesc.MipLevels = 1;		// mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// TextureのFormat
    resourceDesc.SampleDesc.Count = 1;		// サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知


    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;	// 1.0f(最大値)でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// フォーマット。Resourceと合わせる

    // Resource生成
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE, // データ転送される設定
        &depthClearValue, // Clear最適値
        IID_PPV_ARGS(&resource_)); // 作成するResourceポインタへのポインタ
    assert(SUCCEEDED(hr));
    (void)hr;

    assert(SUCCEEDED(hr));
}

void DepthStencil::CreateDepthStencilView(ID3D12Device *device) {
    dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    device->CreateDepthStencilView(
        resource_.Get(), &dsvDesc_, dsvHeap_.GetCPUDescriptorHandle(0)
    );
}
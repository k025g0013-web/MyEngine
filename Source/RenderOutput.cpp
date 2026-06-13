#include "RenderOutput.h"

#include "DirectXDevice.h"
#include "CommandContext.h"
#include "WinApp.h"

#include <cassert>

void RenderOutput::Initialize(
    DirectXDevice *device, CommandContext *commandContext, WinApp *winApp,
    DescriptorHeap *descriptorHeap, uint32_t width, uint32_t height
) {
    ID3D12Device *d3d12Device = device->GetDevice();

    //====================
    // SwapChain生成
    //====================
    swapChainDesc_.Width = width;
    swapChainDesc_.Height = height;
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc_.SampleDesc.Count = 1;
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc_.BufferCount = kBufferCount;
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    // コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    HRESULT hr = device->GetDXGIFactory()->CreateSwapChainForHwnd(
        commandContext->GetCommandQueue(), winApp->GetHWND(),
        &swapChainDesc_, nullptr, nullptr,
        reinterpret_cast<IDXGISwapChain1 **>(swapChain_.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // SwapChainからResourceを引っ張ってくる
    for (uint32_t i = 0; i < kBufferCount; ++i) {
        hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(backBuffers_[i].GetAddressOf()));
        assert(SUCCEEDED(hr));
    }

    //====================
    // RTV生成
    //====================
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// 出力結果をSRGBに変換して書き込む
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // 2つのディスクリプタハンドルを得る 
    for (uint32_t i = 0; i < kBufferCount; ++i) {
        // スワップチェーンからリソースを取得
        hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(backBuffers_[i].GetAddressOf()));
        assert(SUCCEEDED(hr));

        // ディスクリプタの先頭を取得する
        rtvHandles_[i] = descriptorHeap->GetCPUDescriptorHandle(i);

        // 2つ目のディスクリプタハンドルを得る
        d3d12Device->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc_, rtvHandles_[i]);
    }

    //====================
    // DSV生成
    //====================
    CreateDepthStencil(d3d12Device, width, height);

    (void)hr;
}

void RenderOutput::CreateDepthStencil(ID3D12Device *device, uint32_t width, uint32_t height) {
    //====================
    // DSVHeap生成
    //====================
    dsvHeap_.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

    //====================
    // Resource
    //====================
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

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;	// 1.0f(最大値)でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Resource生成
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, 
        D3D12_HEAP_FLAG_NONE, 
        &resourceDesc, 
        D3D12_RESOURCE_STATE_DEPTH_WRITE, 
        &depthClearValue, 
        IID_PPV_ARGS(&depthStencilResource_)); 
    assert(SUCCEEDED(hr));

    //====================
    // DSV記述子の生成と保持
    //====================
    dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    device->CreateDepthStencilView(
        depthStencilResource_.Get(), &dsvDesc_, dsvHeap_.GetCPUDescriptorHandle(0)
    );

    (void)hr;
}

void RenderOutput::Present() {
    swapChain_->Present(1, 0);
}
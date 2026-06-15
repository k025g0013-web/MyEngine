#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <array>

#include "Graphics/Core/DescriptorHeap.h" 

class DirectXDevice;
class CommandContext;
class WinApp;

class RenderOutput {
public:
    static const uint32_t kBufferCount = 2;

    void Initialize(
        DirectXDevice *device, CommandContext *commandContext, WinApp *winApp,
        DescriptorHeap *rtvDescriptorHeap, uint32_t width, uint32_t height
    );

    // GPUとOSに画面の交換を行うよう通知する
    void Present();

    // === getter ===
    uint32_t GetBufferCount() const { return kBufferCount; }

    // SwapChain
    IDXGISwapChain4 *GetSwapChain() const { return swapChain_.Get(); }
    UINT GetCurrentBackBufferIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }
    const DXGI_SWAP_CHAIN_DESC1 &GetDesc() const { return swapChainDesc_; }

    ID3D12Resource *GetBackBuffer(uint32_t index) const { return backBuffers_[index].Get(); }
    ID3D12Resource *GetCurrentBackBuffer() const { return backBuffers_[GetCurrentBackBufferIndex()].Get(); }

    // RenderTargetView
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t index) const { return rtvHandles_[index]; }
    const D3D12_RENDER_TARGET_VIEW_DESC &GetRTVDesc() const { return rtvDesc_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const { return rtvHandles_[GetCurrentBackBufferIndex()]; }

    // DepthStencil
    ID3D12Resource *GetResource() const { return depthStencilResource_.Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvHeap_.GetCPUDescriptorHandle(0); }

private:
    // DSV生成
    void CreateDepthStencil(ID3D12Device *device, uint32_t width, uint32_t height);

private:
    // === SwapChain ===
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kBufferCount> backBuffers_;

    // === RenderTargetView ===
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> rtvHandles_{};
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

    // === DepthStencil ===
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
    DescriptorHeap dsvHeap_;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};
};
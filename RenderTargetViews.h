#pragma once

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

#include <array>

class SwapChain;
class DescriptorHeap;

class RenderTargetViews {
public:
    static const uint32_t kRenderTargetCount = 2;

    // RTVの設定
    void Initialize(
        ID3D12Device* device, SwapChain* swapChain, DescriptorHeap* descriptorHeap
    );

    // getter
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t index) const {return rtvHandles_[index];}
    const D3D12_RENDER_TARGET_VIEW_DESC& GetRTVDesc() const {return rtvDesc_;}

private:
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kRenderTargetCount> rtvHandles_{};
};
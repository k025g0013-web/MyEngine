#pragma once

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <d3d12.h>
#include <dxgi1_6.h>

#include <cstdint>

class DirectXDevice;
class CommandContext;
class WinApp;

class SwapChain {
public:
    // スワップチェーン生成
    void Initialize(
        DirectXDevice* device, CommandContext* commandContext, WinApp* winApp, 
        uint32_t width, uint32_t height);

    void Present(); // GPUとOSに画面の交換を行うよう通知する

    // getter
    IDXGISwapChain4* GetSwapChain() const {return swapChain_.Get();}
    ID3D12Resource* GetBackBuffer(uint32_t index) const {return backBuffers_[index].Get();}
    UINT GetCurrentBackBufferIndex() const {return swapChain_->GetCurrentBackBufferIndex();}
    const DXGI_SWAP_CHAIN_DESC1& GetDesc() const {return swapChainDesc_;}
    uint32_t GetBufferCount() const { return kBufferCount; }

private:
    static const uint32_t kBufferCount = 2;

    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers_[kBufferCount];
};
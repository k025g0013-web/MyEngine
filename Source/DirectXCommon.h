#pragma once

#include "DirectXDevice.h"      // Device
#include "CommandContext.h"     // Command系統

#include "RenderOutput.h"

#include "DescriptorHeap.h"     // DescriptorHeap
#include "Fence.h"              // Fence
#include "ViewportState.h" // Viewport/Scissor

class WinApp;
class Logger;
class Shader;

class DirectXCommon {
public:
    void Initialize(
        WinApp* winApp, Logger* logger, uint32_t width, uint32_t height
    );
    void Finalize();

    void BeginFrame();
    void EndFrame();

    // === getter ===
    // Device
    ID3D12Device* GetDevice() const {return directXDevice_.GetDevice();}

    // Command
    ID3D12GraphicsCommandList* GetCommandList() const {return commandContext_.GetCommandList();}
    ID3D12CommandQueue* GetCommandQueue() const {return commandContext_.GetCommandQueue();}
    
    // DescriptorHeap
    DescriptorHeap* GetSRVHeap() {return &srvDescriptorHeap_;}
    DescriptorHeap* GetRTVHeap() {return &rtvDescriptorHeap_;}
    
    // RenderOutput(SwapChain/RTV/DSV)
    RenderOutput *GetRenderOutput() { return &renderOutput_; }

    // Other
    Fence* GetFence() {return &fence_;}

private:
    // === Core ===
    DirectXDevice directXDevice_;
    CommandContext commandContext_;
    
    // === DescriptorHeap ===
    DescriptorHeap rtvDescriptorHeap_;
    DescriptorHeap srvDescriptorHeap_;

    // === RenderOutput(SwapChain/RTV/DSV) ===
    RenderOutput renderOutput_;
    
    // === Other ===
    Fence fence_;
    ViewportState viewportState_{};
};
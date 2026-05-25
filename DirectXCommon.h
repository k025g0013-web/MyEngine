#pragma once

#include "DirectXDevice.h"      // Device
#include "CommandContext.h"     // Command系統
#include "SwapChain.h"          // SwapChain
#include "DescriptorHeap.h"     // DescriptorHeap
#include "RenderTargetViews.h"  // rtv
#include "Fence.h"              // Fence
#include "CompileShader.h"      // CompileShader
#include "DepthStencil.h"  // DepthStencilState
#include "ViewportState.h" // Viewport/Scissor

class WinApp;
class Logger;

class DirectXCommon {
public:
    void Initialize(
        WinApp* winApp, Logger* logger, uint32_t width, uint32_t height
    );

    void BeginFrame();
    void EndFrame();

    // getter
    ID3D12Device* GetDevice() const {return directXDevice_.GetDevice();}

    ID3D12GraphicsCommandList* GetCommandList() const {return commandContext_.GetCommandList();}
    ID3D12CommandQueue* GetCommandQueue() const {return commandContext_.GetCommandQueue();}
    
    SwapChain* GetSwapChain() {return &swapChain_;}
    
    Fence* GetFence() {return &fence_;}
    
    DescriptorHeap* GetSRVHeap() {return &srvDescriptorHeap_;}
    DescriptorHeap* GetRTVHeap() {return &rtvDescriptorHeap_;}
    
    RenderTargetViews* GetRenderTargetView() { return &renderTargetViews_;}
    
    CompileShader* GetCompileShader() {return &compileShader_;}

    DepthStencil *GetDepthStencil() { return &depthStencil_; }

private:
    DirectXDevice directXDevice_;
    
    CommandContext commandContext_;
    
    SwapChain swapChain_;

    DescriptorHeap rtvDescriptorHeap_;
    DescriptorHeap srvDescriptorHeap_;

    RenderTargetViews renderTargetViews_;

    DepthStencil depthStencil_;

    Fence fence_;

    CompileShader compileShader_;

    ViewportState viewportState_;
};
#include "SwapChain.h"

#include "DirectXDevice.h"
#include "CommandContext.h"
#include "WinApp.h"

#include <cassert>

// スワップチェーン生成
void SwapChain::Initialize(
    DirectXDevice* device, CommandContext* commandContext, WinApp* winApp, 
    uint32_t width, uint32_t height) {

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = width;		// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Height = height;	    // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
    swapChainDesc.SampleDesc.Count = 1;	// マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
    swapChainDesc.BufferCount = kBufferCount;	// ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタにうつしたら、中身を破棄

    // コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    HRESULT hr = device->GetDXGIFactory()->CreateSwapChainForHwnd(
        commandContext->GetCommandQueue(), winApp->GetHWND(),
        &swapChainDesc, nullptr, nullptr, 
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // SwapChainからResourceを引っ張ってくる
    for (uint32_t i = 0; i < kBufferCount; ++i) {
        hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(backBuffers_[i].GetAddressOf()));
        assert(SUCCEEDED(hr));
    }
}

// GPUとOSに画面の交換を行うよう通知する
void SwapChain::Present() {
    swapChain_->Present(1, 0);
}
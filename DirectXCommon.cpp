#include "DirectXCommon.h"

void DirectXCommon::Initialize(
    WinApp* winApp, Logger* logger, uint32_t width, uint32_t height
) {
    // Device
    directXDevice_.Initialize(logger);

    // Command系統
    commandContext_.Initialize(&directXDevice_);

    // SwapChain
    swapChain_.Initialize(&directXDevice_, &commandContext_, winApp, width, height);

    // DescriptorHeap(rtv)
    rtvDescriptorHeap_.Initialize(directXDevice_.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

    // DescriptorHeap(srv)
    srvDescriptorHeap_.Initialize(directXDevice_.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

    // rtv
    renderTargetViews_.Initialize(directXDevice_.GetDevice(), &swapChain_, &rtvDescriptorHeap_);

    // Fence
    fence_.Initialize(directXDevice_.GetDevice());

    // CompileShader
    compileShader_.Initialize(logger);
}

void DirectXCommon::BeginFrame() {

}

void DirectXCommon::EndFrame() {
    // コマンドリストの内容を確定させる。すべてのコマンドを詰んでからClearすること
    commandContext_.Close();

    // GPUにコマンドリストの実行を行わせる
    commandContext_.Execute();

    // GPUとOSに画面の交換を行うよう通知する
    swapChain_.Present();

    fence_.Wait(
        commandContext_.GetCommandQueue()
    );

    // 次のフレーム用のコマンドリストを準備
    commandContext_.Reset();
}
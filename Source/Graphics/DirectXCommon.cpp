#include "DirectXCommon.h"

#include "Graphics/Resource/Shader.h"
#include "Graphics/Pipeline/ViewportState.h"

void DirectXCommon::Initialize(
    WinApp* winApp, Logger* logger, uint32_t width, uint32_t height
) {
    // Device
    directXDevice_.Initialize(logger);

    // Command
    commandContext_.Initialize(&directXDevice_);

    // DescriptorHeap
    rtvDescriptorHeap_.Initialize(directXDevice_.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    srvDescriptorHeap_.Initialize(directXDevice_.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

    // RenderOutput(SwapChain/RTV/DSV)
    renderOutput_.Initialize(&directXDevice_, &commandContext_, winApp, &rtvDescriptorHeap_, width, height);

    // Other
    fence_.Initialize(directXDevice_.GetDevice());
    Shader::InitializeCompiler(logger);
    viewportState_.Initialize(static_cast<float>(width), static_cast<float>(height));
}

void DirectXCommon::Finalize() {
}

void DirectXCommon::BeginFrame() {
    ID3D12GraphicsCommandList *commandList = commandContext_.GetCommandList();

    // SwapChain/RTV/DSV
    ID3D12Resource *currentBackBuffer = renderOutput_.GetCurrentBackBuffer();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderOutput_.GetCurrentRTVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderOutput_.GetDSVHandle();

    // TransitionBarrierの設定 (PRESENT -> RENDER_TARGET)
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = currentBackBuffer;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    // TransitionBarrierを張る
    commandList->ResourceBarrier(1, &barrier);

    // 描画先のRTVとDSVを設定する
    commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    // 指定した色で画面全体をクリアする
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // 指定した深度で画面全体をクリアする
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // 描画用DescriptorHeapの設定
    ID3D12DescriptorHeap *descriptorHeaps[] = { srvDescriptorHeap_.GetDescriptorHeap() };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);

    // Viewport/Scissorを設定
    viewportState_.SetCommand(commandList);   
}

void DirectXCommon::EndFrame() {
    ID3D12GraphicsCommandList *commandList = commandContext_.GetCommandList();

    // 現在のバックバッファを取得
    ID3D12Resource *currentBackBuffer = renderOutput_.GetCurrentBackBuffer();

    // TransitionBarrierの設定
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = currentBackBuffer; 
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    // TransitionBarrierを張る
    commandList->ResourceBarrier(1, &barrier);

    // コマンドリストの内容を確定させる
    commandContext_.Close();

    // GPUにコマンドリストの実行を行わせる
    commandContext_.Execute();

    // GPUとOSに画面の交換を行うよう通知する
    renderOutput_.Present();

    // 同期処理
    fence_.Wait(commandContext_.GetCommandQueue());

    // 次のフレーム用のコマンドリストを準備
    commandContext_.Reset();
}
#include "DirectXCommon.h"

#include "ViewportState.h"

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

    // dsv
    depthStencil_.Initialize(directXDevice_.GetDevice(), width, height);

    // Fence
    fence_.Initialize(directXDevice_.GetDevice());

    // CompileShader
    compileShader_.Initialize(logger);

    // Viewport/ScissorRect
    viewportState_.Initialize(
        static_cast<float>(width),
        static_cast<float>(height)
    );
}

void DirectXCommon::BeginFrame() {
    // これから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = swapChain_.GetCurrentBackBufferIndex();

    // TransitionBarrierの設定
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;                      // 今回のバリアはTranslation
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;                           // Noneにしておく
    barrier.Transition.pResource = swapChain_.GetBackBuffer(backBufferIndex);   // バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;              // 遷移前(現在)のResourceState
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;         // 遷移後のResourceState

    // TransitionBarrierを張る
    commandContext_.GetCommandList()->ResourceBarrier(1, &barrier);

    // 描画先のRTVとDSVを設定する
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_.GetDSVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTargetViews_.GetRTVHandle(backBufferIndex);

    commandContext_.GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
    // 指定した色で画面全体をクリアする
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
    commandContext_.GetCommandList()->ClearRenderTargetView(renderTargetViews_.GetRTVHandle(backBufferIndex), clearColor, 0, nullptr);

    // 指定した深度で画面全体をクリアする
    commandContext_.GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // 描画用DescriptorHeapの設定
    ID3D12DescriptorHeap *descriptorHeaps[] = { srvDescriptorHeap_.GetDescriptorHeap() };
    commandContext_.GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    // Viewport/Scissorを設定
    viewportState_.SetCommand(commandContext_.GetCommandList());
}

void DirectXCommon::EndFrame() {
    UINT backBufferIndex = swapChain_.GetCurrentBackBufferIndex();

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;                      // 今回のバリアはTranslation
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;                           // Noneにしておく
    barrier.Transition.pResource = swapChain_.GetBackBuffer(backBufferIndex);   // バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;        // 今回はRenderTargetからPresentにする
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;               

    // TransitionBarrierを張る
    commandContext_.GetCommandList()->ResourceBarrier(1, &barrier);

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
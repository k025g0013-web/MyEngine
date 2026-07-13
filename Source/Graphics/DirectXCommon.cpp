#include "DirectXCommon.h"

#include "Graphics/Resource/Shader.h"
#include "Graphics/Pipeline/ViewportState.h"

void DirectXCommon::Initialize(
    WinApp* winApp, Logger* logger, uint32_t width, uint32_t height
) {
    // DirectXデバイスを生成する
    directXDevice_.Initialize(logger);

    // コマンドキュー・コマンドリストを生成する
    commandContext_.Initialize(&directXDevice_);

    // RTV・SRV用DescriptorHeapを生成する
    rtvDescriptorHeap_.Initialize(directXDevice_.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    srvDescriptorHeap_.Initialize(directXDevice_.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

    // SwapChain・RTV・DSVなど描画先を生成する
    renderOutput_.Initialize(&directXDevice_, &commandContext_, winApp, &rtvDescriptorHeap_, width, height);

    // GPU同期オブジェクトを生成する
    fence_.Initialize(directXDevice_.GetDevice());

    // シェーダコンパイラを初期化する
    Shader::InitializeCompiler(logger);

    // Viewport・Scissorを設定する
    viewportState_.Initialize(static_cast<float>(width), static_cast<float>(height));
}

void DirectXCommon::Finalize() {
    // 現在は各メンバクラスがRAIIで解放するため明示的な終了処理は不要
}

void DirectXCommon::BeginFrame() {
    ID3D12GraphicsCommandList *commandList = commandContext_.GetCommandList();

    // 現在描画対象となるバックバッファを取得する
    ID3D12Resource *currentBackBuffer = renderOutput_.GetCurrentBackBuffer();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderOutput_.GetCurrentRTVHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderOutput_.GetDSVHandle();

    // バックバッファを描画可能状態へ遷移させる
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = currentBackBuffer;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    // リソースバリアを適用する
    commandList->ResourceBarrier(1, &barrier);

    // 描画先RTV・DSVを設定する
    commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    // カラーバッファを指定色で初期化する
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // DepthBufferを初期化する
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // 描画で使用するDescriptorHeapを設定する
    ID3D12DescriptorHeap *descriptorHeaps[] = { srvDescriptorHeap_.GetDescriptorHeap() };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);

    // Viewport・Scissorを適用する
    viewportState_.SetCommand(commandList);
}

void DirectXCommon::EndFrame() {
    ID3D12GraphicsCommandList *commandList = commandContext_.GetCommandList();

    // 現在のバックバッファを取得する
    ID3D12Resource *currentBackBuffer = renderOutput_.GetCurrentBackBuffer();

    // バックバッファを画面表示状態へ戻す
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = currentBackBuffer;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    // リソースバリアを適用する
    commandList->ResourceBarrier(1, &barrier);

    // コマンドリストを確定する
    commandContext_.Close();

    // GPUへ描画コマンドを送信する
    commandContext_.Execute();

    // SwapChainをPresentし画面を更新する
    renderOutput_.Present();

    // GPU処理の完了を待機する
    fence_.Wait(commandContext_.GetCommandQueue());

    // 次フレーム用にコマンドリストを初期状態へ戻す
    commandContext_.Reset();
}
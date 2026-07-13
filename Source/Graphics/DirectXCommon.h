#pragma once

// Graphic/Core
#include "Graphics/Core/DirectXDevice.h"
#include "Graphics/Core/CommandContext.h"
#include "Graphics/Core/Fence.h"
#include "Graphics/Core/DescriptorHeap.h"

// Graphic/Pipeline
#include "Graphics/Pipeline/ViewportState.h"

// Graphics/Frame
#include "Graphics/Frame/RenderOutput.h"

class WinApp;
class Logger;
class Shader;

/// <summary>
/// DirectX12の描画基盤を管理するクラス
/// </summary>
/// <remarks>
/// DirectXデバイス、コマンドリスト、SwapChain、
/// DescriptorHeap、Fenceなど描画に必要な共通機能を管理する。
/// フレーム開始・終了処理も本クラスが担当する。
/// </remarks>
class DirectXCommon {
public:

    /// <summary>
    /// DirectX共通システムを初期化する
    /// </summary>
    /// <param name="winApp">ウィンドウ管理クラス</param>
    /// <param name="logger">ログ出力クラス</param>
    /// <param name="width">画面幅</param>
    /// <param name="height">画面高さ</param>
    void Initialize(
        WinApp *winApp, Logger *logger, uint32_t width, uint32_t height
    );

    /// <summary>
    /// DirectX共通システムを終了する
    /// </summary>
    void Finalize();

    /// <summary>
    /// フレーム開始時の描画準備を行う
    /// </summary>
    /// <remarks>
    /// バックバッファの遷移、RTV・DSV設定、
    /// 画面クリア、Viewport設定などを行う。
    /// </remarks>
    void BeginFrame();

    /// <summary>
    /// フレーム終了処理を行う
    /// </summary>
    /// <remarks>
    /// バックバッファの状態をPresentへ戻し、
    /// コマンド実行、画面更新、GPU同期を行う。
    /// </remarks>
    void EndFrame();

    /// <summary>
    /// DirectXデバイスを取得する
    /// </summary>
    /// <returns>DirectXデバイス</returns>
    ID3D12Device *GetDevice() const { return directXDevice_.GetDevice(); }

    /// <summary>
    /// グラフィックスコマンドリストを取得する
    /// </summary>
    /// <returns>コマンドリスト</returns>
    ID3D12GraphicsCommandList *GetCommandList() const { return commandContext_.GetCommandList(); }

    /// <summary>
    /// コマンドキューを取得する
    /// </summary>
    /// <returns>コマンドキュー</returns>
    ID3D12CommandQueue *GetCommandQueue() const { return commandContext_.GetCommandQueue(); }

    /// <summary>
    /// SRV用DescriptorHeapを取得する
    /// </summary>
    /// <returns>SRV DescriptorHeap</returns>
    DescriptorHeap *GetSRVHeap() { return &srvDescriptorHeap_; }

    /// <summary>
    /// RTV用DescriptorHeapを取得する
    /// </summary>
    /// <returns>RTV DescriptorHeap</returns>
    DescriptorHeap *GetRTVHeap() { return &rtvDescriptorHeap_; }

    /// <summary>
    /// RenderOutputを取得する
    /// </summary>
    /// <returns>RenderOutputクラス</returns>
    RenderOutput *GetRenderOutput() { return &renderOutput_; }

    /// <summary>
    /// Fenceを取得する
    /// </summary>
    /// <returns>Fenceクラス</returns>
    Fence *GetFence() { return &fence_; }

private:
    /// DirectXデバイス
    DirectXDevice directXDevice_;

    /// コマンド管理
    CommandContext commandContext_;

    /// RTV用DescriptorHeap
    DescriptorHeap rtvDescriptorHeap_;

    /// SRV用DescriptorHeap
    DescriptorHeap srvDescriptorHeap_;

    /// SwapChain・RTV・DSV管理
    RenderOutput renderOutput_;

    /// GPU同期用Fence
    Fence fence_;

    /// Viewport・Scissor管理
    ViewportState viewportState_{};
};
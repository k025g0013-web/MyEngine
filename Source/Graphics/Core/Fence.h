#pragma once
#include <Windows.h>
#include <wrl.h>

#include <cstdint>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

/// <summary>
/// GPUとCPU間の同期処理を管理するクラス
/// </summary>
/// <remarks>
/// DirectX12のFence機能を利用し、GPUのコマンド実行完了を検知する。
/// CPU側がGPU処理の完了を待機することで、リソースの安全な利用や
/// フレーム間の同期処理を実現する。
/// </remarks>
class Fence {
public:

    /// <summary>
    /// Fence管理を終了する
    /// </summary>
    /// <remarks>
    /// GPU待機用に生成したイベントハンドルを解放する。
    /// </remarks>
    ~Fence();


    /// <summary>
    /// Fenceを初期化する
    /// </summary>
    /// <remarks>
    /// GPU処理完了を管理するFenceリソースと、
    /// 完了通知を受け取るためのイベントを生成する。
    /// </remarks>
    /// <param name="device">DirectX12デバイス</param>
    void Initialize(ID3D12Device *device);


    /// <summary>
    /// GPUの処理完了を待機する
    /// </summary>
    /// <remarks>
    /// CommandQueueへSignalを送信し、
    /// 指定したFence値へGPUが到達するまでCPU側を待機させる。
    /// </remarks>
    /// <param name="commandQueue">GPUコマンド実行用CommandQueue</param>
    void Wait(ID3D12CommandQueue *commandQueue);


private:

    /// GPUとCPUの同期状態を管理するFenceリソース
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;


    /// GPU完了位置を管理するFence値
    uint64_t fenceValue_ = 0;


    /// Fence完了通知を受け取るWindowsイベント
    HANDLE fenceEvent_ = nullptr;
};
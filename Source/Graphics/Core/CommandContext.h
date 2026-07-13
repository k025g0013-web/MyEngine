#pragma once

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

class DirectXDevice;

/// <summary>
/// DirectX12のコマンド関連機能を管理するクラス
/// </summary>
/// <remarks>
/// GPUへ送信する描画命令を管理するCommandQueue、
/// コマンドを記録するCommandAllocator、CommandListの生成と制御を行う。
/// フレームごとのコマンド記録、実行、リセット処理を担当する。
/// </remarks>
class CommandContext {
public:

    /// <summary>
    /// コマンド関連リソースを初期化する
    /// </summary>
    /// <param name="device">
    /// DirectX12デバイス
    /// </param>
    void Initialize(DirectXDevice *device);


    /// <summary>
    /// コマンドリストの記録を終了する
    /// </summary>
    /// <remarks>
    /// 記録したGPUコマンドを実行可能な状態へ確定する。
    /// Execute前に呼び出す必要がある。
    /// </remarks>
    void Close();


    /// <summary>
    /// コマンドリストをGPUへ送信する
    /// </summary>
    /// <remarks>
    /// CommandQueueへコマンドリストを渡し、
    /// GPUによる処理を開始させる。
    /// </remarks>
    void Execute();


    /// <summary>
    /// コマンド記録状態をリセットする
    /// </summary>
    /// <remarks>
    /// 新しいフレームのコマンド記録を開始できる状態へ戻す。
    /// </remarks>
    void Reset();


    /// <summary>
    /// コマンドキューを取得する
    /// </summary>
    /// <returns>
    /// ID3D12CommandQueue
    /// </returns>
    ID3D12CommandQueue *GetCommandQueue() const { return commandQueue_.Get(); }


    /// <summary>
    /// グラフィックスコマンドリストを取得する
    /// </summary>
    /// <returns>
    /// ID3D12GraphicsCommandList
    /// </returns>
    ID3D12GraphicsCommandList *GetCommandList() const { return commandList_.Get(); }


private:

    /// <summary>
    /// コマンドキューを生成する
    /// </summary>
    /// <remarks>
    /// GPUへコマンドを送信するためのCommandQueueを作成する。
    /// </remarks>
    void CreateCommandQueue();


    /// <summary>
    /// コマンドアロケータを生成する
    /// </summary>
    /// <remarks>
    /// CommandListが記録するコマンド領域を管理する
    /// CommandAllocatorを作成する。
    /// </remarks>
    void CreateCommandAllocator();


    /// <summary>
    /// コマンドリストを生成する
    /// </summary>
    /// <remarks>
    /// GPUへ送信する描画命令を記録する
    /// GraphicsCommandListを作成する。
    /// </remarks>
    void CreateCommandList();


private:

    /// DirectX12デバイス
    DirectXDevice *device_ = nullptr;


    /// GPUへコマンドを送信するCommandQueue
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;


    /// コマンド記録領域を管理するCommandAllocator
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;


    /// 描画命令を記録するGraphicsCommandList
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
};
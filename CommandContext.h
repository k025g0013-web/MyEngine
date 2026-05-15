#pragma once

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

class DirectXDevice;

class CommandContext {
public:
    void Initialize(DirectXDevice* device);

    void Close();   // コマンドリストの内容を確定させる
    void Execute(); // GPUにコマンドリストの実行を行わせる
    void Reset();   // コマンドリストリセット

    // getter
    ID3D12CommandQueue* GetCommandQueue() const {return commandQueue_.Get();}
    ID3D12GraphicsCommandList* GetCommandList() const {return commandList_.Get();}

private:
    // コマンドキュー生成
    void CreateCommandQueue();

    // コマンドアロケータ生成
    void CreateCommandAllocator();

    // コマンドリスト生成
    void CreateCommandList();

private:
    DirectXDevice* device_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
};
#pragma once
#include <Windows.h>
#include <wrl.h>

#include <cstdint>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

class Fence {
public:
    ~Fence();

    // Fence生成
    void Initialize(ID3D12Device* device);

    // GPUの実行待ち
    void Wait(ID3D12CommandQueue* commandQueue);

private:
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

    uint64_t fenceValue_ = 0;

    HANDLE fenceEvent_ = nullptr;
};
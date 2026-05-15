#pragma once

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

#include <cstdint>

class DirectXDevice;

class DescriptorHeap {
public:
    // DescriptorHeap生成
    void Initialize(
        ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible
    );

    // getter
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) const;
    uint32_t GetDescriptorSize() const { return descriptorSize_; }

private:
    uint32_t descriptorSize_ = 0;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
};
#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <vector>

#pragma comment(lib, "d3d12.lib")

namespace Kizuna {

    class DirectXDevice;

    /// <summary>
    /// DirectX12のDescriptorHeapを管理するクラス
    /// </summary>
    class DescriptorHeap {
    public:
        void Initialize(
            ID3D12Device *device,
            D3D12_DESCRIPTOR_HEAP_TYPE heapType,
            UINT numDescriptors,
            bool shaderVisible
        );

        ID3D12DescriptorHeap *GetDescriptorHeap() const {
            return descriptorHeap_.Get();
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
            uint32_t index
        ) const;

        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
            uint32_t index
        ) const;

        uint32_t GetDescriptorSize() const {
            return descriptorSize_;
        }

        // Descriptorを割り当てる
        uint32_t AllocateDescriptor();

        // Descriptorを解放して再利用可能にする
        void FreeDescriptor(uint32_t index);

        // 次に新規割り当てするDescriptor番号
        uint32_t nextDescriptorIndex_ = 1;

        // 作成したDescriptorの総数
        uint32_t descriptorCount_ = 0;

    private:
        // Descriptor1つあたりのサイズ
        uint32_t descriptorSize_ = 0;

        // Descriptorを管理するHeap
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

        // 解放済みDescriptorのインデックスを管理する
        std::vector<uint32_t> freeDescriptorIndices_;

        // 各インデックスが現在割り当て済みかどうか
        std::vector<bool> isDescriptorAllocated_;
    };
}
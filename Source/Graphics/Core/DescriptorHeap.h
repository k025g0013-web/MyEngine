#pragma once

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

#include <cstdint>

namespace Kizuna {
    class DirectXDevice;

    /// <summary>
    /// DirectX12のDescriptorHeapを管理するクラス
    /// </summary>
    /// <remarks>
    /// CBV、SRV、UAV、RTV、DSVなどのDescriptorを管理するHeapを生成し、
    /// CPUおよびGPUからアクセスするためのDescriptorHandleを提供する。
    /// </remarks>
    class DescriptorHeap {
    public:

        /// <summary>
        /// DescriptorHeapを初期化する
        /// </summary>
        /// <remarks>
        /// 指定された種類のDescriptorHeapを生成し、
        /// Descriptorのサイズを取得して保持する。
        /// </remarks>
        /// <param name="device">DirectX12デバイス</param>
        /// <param name="heapType">生成するDescriptorHeapの種類</param>
        /// <param name="numDescriptors">作成するDescriptor数</param>
        /// <param name="shaderVisible">シェーダーから参照可能にするか</param>
        void Initialize(
            ID3D12Device *device,
            D3D12_DESCRIPTOR_HEAP_TYPE heapType,
            UINT numDescriptors,
            bool shaderVisible
        );


        /// <summary>
        /// DescriptorHeapを取得する
        /// </summary>
        /// <returns>DescriptorHeap</returns>
        ID3D12DescriptorHeap *GetDescriptorHeap() const {
            return descriptorHeap_.Get();
        }


        /// <summary>
        /// 指定した位置のCPU用DescriptorHandleを取得する
        /// </summary>
        /// <param name="index">取得するDescriptor番号</param>
        /// <returns>CPU DescriptorHandle</returns>
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) const;


        /// <summary>
        /// 指定した位置のGPU用DescriptorHandleを取得する
        /// </summary>
        /// <param name="index">取得するDescriptor番号</param>
        /// <returns>GPU DescriptorHandle</returns>
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) const;


        /// <summary>
        /// Descriptor1つ分のサイズを取得する
        /// </summary>
        /// <returns>Descriptorサイズ</returns>
        uint32_t GetDescriptorSize() const {
            return descriptorSize_;
        }


    private:

        /// Descriptor1つあたりのサイズ
        uint32_t descriptorSize_ = 0;


        /// Descriptorを管理するHeap
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
    };
}
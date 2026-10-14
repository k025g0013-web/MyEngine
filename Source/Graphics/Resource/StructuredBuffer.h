
#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstddef>
#include <cstdint>

namespace Kizuna {

    class DescriptorHeap;

    /// <summary>
    /// 構造化バッファを管理するクラス
    /// </summary>
    /// <remarks>
    /// GPU上に構造化バッファを作成し、
    /// SRVを通してシェーダーから読み取れるようにする。
    /// </remarks>
    class StructuredBuffer {
    public:

        /// <summary>
        /// 構造化バッファを初期化する
        /// </summary>
        /// <param name="device">DirectX12デバイス</param>
        /// <param name="srvHeap">SRVを登録するDescriptorHeap</param>
        /// <param name="descriptorIndex">SRVを登録するDescriptor番号</param>
        /// <param name="elementSize">要素1つ分のバイト数</param>
        /// <param name="elementCount">要素数</param>
        void Initialize(
            ID3D12Device *device,
            DescriptorHeap *srvHeap,
            uint32_t descriptorIndex,
            uint32_t elementSize,
            uint32_t elementCount
        );

        /// <summary>
        /// バッファのデータを更新する
        /// </summary>
        /// <param name="data">書き込むデータの先頭アドレス</param>
        /// <param name="dataSize">書き込むデータのバイト数</param>
        void Update(
            const void *data,
            size_t dataSize
        );

        /// <summary>
        /// SRVのGPUハンドルを取得する
        /// </summary>
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const {
            return gpuHandle_;
        }

        /// <summary>
        /// GPUリソースを取得する
        /// </summary>
        ID3D12Resource *GetResource() const {
            return resource_.Get();
        }

        /// <summary>
        /// 1要素あたりのサイズを取得する
        /// </summary>
        uint32_t GetElementSize() const {
            return elementSize_;
        }

        /// <summary>
        /// 要素数を取得する
        /// </summary>
        uint32_t GetElementCount() const {
            return elementCount_;
        }

    private:

        /// GPU上のバッファリソース
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

        /// バッファにCPUから書き込むためのアドレス
        void *mappedData_ = nullptr;

        /// SRVのGPUハンドル
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};

        /// 1要素あたりのサイズ（バイト）
        uint32_t elementSize_ = 0;

        /// バッファ内の要素数
        uint32_t elementCount_ = 0;
    };
}

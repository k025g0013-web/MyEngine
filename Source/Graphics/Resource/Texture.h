#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXTex/DirectXTex.h>

#include <cstdint>

namespace Kizuna {

    /// <summary>
    /// GPU上のテクスチャを参照するための情報
    /// </summary>
    struct TextureData {
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
        DirectX::TexMetadata metadata{};
        ID3D12Resource *resource = nullptr;
    };

    /// <summary>
    /// 1つのGPUテクスチャを管理するクラス
    /// </summary>
    class Texture {
    public:

        /// <summary>
        /// 読み込まれたテクスチャ情報を設定する
        /// </summary>
        void SetData(
            const TextureData &data);

        /// <summary>
        /// テクスチャ情報を取得する
        /// </summary>
        const TextureData &GetData() const {
            return data_;
        }

        /// <summary>
        /// テクスチャのGPUハンドルを取得する
        /// </summary>
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const {
            return data_.gpuHandle;
        }

        /// <summary>
        /// テクスチャリソースを取得する
        /// </summary>
        ID3D12Resource *GetResource() const {
            return resource_.Get();
        }

        /// <summary>
        /// GPUリソースを設定する
        /// </summary>
        void SetResource(
            Microsoft::WRL::ComPtr<ID3D12Resource> resource);

        /// <summary>
        /// GPU転送用の中間バッファを設定する
        /// </summary>
        void SetIntermediate(
            Microsoft::WRL::ComPtr<ID3D12Resource> intermediate);

    private:

        /// GPU上のテクスチャリソース
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

        /// GPU転送用の中間バッファ
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediate_;

        /// テクスチャ情報
        TextureData data_{};
    };
}
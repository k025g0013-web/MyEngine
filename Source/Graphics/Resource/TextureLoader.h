#pragma once

#include <d3d12.h>

#include <DirectXTex/DirectXTex.h>
#include <wrl.h>

#include <string>

#include "Texture.h"

namespace Kizuna {

    class DescriptorHeap;

    /// <summary>
    /// テクスチャをファイルから読み込むクラス
    /// </summary>
    class TextureLoader {
    public:

        /// <summary>
        /// テクスチャローダーを初期化する
        /// </summary>
        void Initialize(
            ID3D12Device *device,
            DescriptorHeap *srvHeap);

        /// <summary>
        /// テクスチャを読み込む
        /// </summary>
        TextureData LoadTexture(
            ID3D12GraphicsCommandList *commandList,
            const std::string &textureName,
            Texture &texture);

    private:

        struct LoadContext {
            DirectX::ScratchImage mipImages{};
            DirectX::TexMetadata metadata{};

            Microsoft::WRL::ComPtr<ID3D12Resource> resource;
            Microsoft::WRL::ComPtr<ID3D12Resource> intermediate;

            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
        };

        bool LoadWICAndGenerateMips(
            const std::string &textureName,
            LoadContext &context);

        bool CreateTextureResource(
            LoadContext &context);

        void UploadTextureData(
            ID3D12GraphicsCommandList *commandList,
            LoadContext &context);

        void CreateSRV(
            LoadContext &context);

    private:

        ID3D12Device *device_ = nullptr;
        DescriptorHeap *srvHeap_ = nullptr;

        static uint32_t nextDescriptorIndex_;
    };
}
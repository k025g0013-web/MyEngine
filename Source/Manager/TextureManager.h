#pragma once

#include <d3d12.h>

#include <map>
#include <memory>
#include <string>

#include "Graphics/Resource/Texture.h"
#include "Graphics/Resource/TextureLoader.h"

namespace Kizuna {

    class DescriptorHeap;

    /// <summary>
    /// テクスチャを管理するクラス
    /// </summary>
    /// <remarks>
    /// 読み込んだTextureを名前で管理し、
    /// 同じテクスチャの重複読み込みを防ぐ。
    /// </remarks>
    class TextureManager {
    public:

        /// <summary>
        /// テクスチャ管理を初期化する
        /// </summary>
        void Initialize(
            ID3D12Device *device,
            DescriptorHeap *srvHeap);

        /// <summary>
        /// テクスチャ管理を終了する
        /// </summary>
        void Finalize();

        /// <summary>
        /// テクスチャを読み込む
        /// </summary>
        /// <param name="commandList">
        /// GPU転送に使用するコマンドリスト
        /// </param>
        /// <param name="textureName">
        /// 読み込むテクスチャファイル名
        /// </param>
        /// <returns>
        /// 読み込んだテクスチャ情報
        /// </returns>
        TextureData LoadTexture(
            ID3D12GraphicsCommandList *commandList,
            const std::string &textureName);

        /// <summary>
        /// テクスチャのGPUハンドルを取得する
        /// </summary>
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(
            const std::string &textureName) const;

        /// <summary>
        /// テクスチャリソースを取得する
        /// </summary>
        ID3D12Resource *GetResource(
            const std::string &textureName) const;

    private:

        /// <summary>
        /// 読み込み済みテクスチャ一覧
        /// </summary>
        std::map<
            std::string,
            std::unique_ptr<Texture>
        > textures_;

        /// <summary>
        /// テクスチャ読み込みクラス
        /// </summary>
        TextureLoader loader_;

        /// DirectXデバイス
        ID3D12Device *device_ = nullptr;

        /// SRV用DescriptorHeap
        DescriptorHeap *srvHeap_ = nullptr;
    };
}
#include "TextureManager.h"

#include <cassert>

#include "Graphics/Core/DescriptorHeap.h"

namespace Kizuna {

    void TextureManager::Initialize(
        ID3D12Device *device,
        DescriptorHeap *srvHeap) {

        assert(device);
        assert(srvHeap);

        device_ = device;
        srvHeap_ = srvHeap;

        // TextureLoaderを初期化する
        loader_.Initialize(
            device_,
            srvHeap_);
    }

    void TextureManager::Finalize() {

        // 管理しているTextureをすべて破棄する
        textures_.clear();

        device_ = nullptr;
        srvHeap_ = nullptr;
    }

    TextureData TextureManager::LoadTexture(
        ID3D12GraphicsCommandList *commandList,
        const std::string &textureName) {

        assert(
            device_ &&
            srvHeap_ &&
            "TextureManagerが初期化されていません");

        // すでに読み込まれている場合は再利用する
        auto it =
            textures_.find(textureName);

        if (it != textures_.end()) {
            return it->second->GetData();
        }

        // Textureを生成する
        auto texture =
            std::make_unique<Texture>();

        // TextureLoaderを使用して
        // テクスチャを読み込む
        TextureData data =
            loader_.LoadTexture(
                commandList,
                textureName,
                *texture);

        // 管理対象へ登録する
        textures_.emplace(
            textureName,
            std::move(texture));

        return data;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE
        TextureManager::GetGPUHandle(
            const std::string &textureName) const {

        auto it =
            textures_.find(textureName);

        if (it != textures_.end()) {
            return it->second->GetGPUHandle();
        }

        assert(
            false &&
            "指定されたテクスチャは読み込まれていません");

        return {};
    }

    ID3D12Resource *
        TextureManager::GetResource(
            const std::string &textureName) const {

        auto it =
            textures_.find(textureName);

        if (it != textures_.end()) {
            return it->second->GetResource();
        }

        assert(
            false &&
            "指定されたテクスチャは読み込まれていません");

        return nullptr;
    }
}
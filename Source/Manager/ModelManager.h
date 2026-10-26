
#pragma once

#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Object/Model/Model.h"
#include "Object/Model/ModelLoader.h"
#include "Graphics/Resource/Texture.h"
#include "Math/Transform.h"

namespace Kizuna {

    class TextureManager;

    class ModelManager {
    public:
        void Initialize(
            ID3D12Device *device,
            TextureManager *textureManager);

        void CreateModel(
            ID3D12GraphicsCommandList *commandList,
            const std::string &modelName,
            const TextureData &texture,
            uint32_t color = 0xFFFFFFFF,
            bool enableLighting = true);

        void CreateModel(
            ID3D12GraphicsCommandList *commandList,
            const std::string &modelName,
            uint32_t color = 0xFFFFFFFF,
            bool enableLighting = true);

        void Update(CameraManager *camera);

        void Draw(ID3D12GraphicsCommandList *commandList);

        void DeleteModel(size_t index);

        void Finalize();

        std::vector<std::unique_ptr<Model>> &GetModels() {
            return models_;
        }

        size_t GetModelCount() const {
            return models_.size();
        }

    private:
        // モデルリソースを取得する
        std::shared_ptr<ModelData> LoadModelResource(
            const std::string &modelName);

    private:
        // モデル名ごとの共有リソース
        std::map<
            std::string,
            std::shared_ptr<ModelData>
        > modelResources_;

        ID3D12Device *device_ = nullptr;
        TextureManager *textureManager_ = nullptr;

        // シーン上の個別インスタンス
        std::vector<std::unique_ptr<Model>> models_;
    };
}

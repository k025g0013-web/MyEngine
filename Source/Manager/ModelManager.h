#pragma once

#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Object/Model/Model.h"
#include "Graphics/Resource/Texture.h"
#include "Math/Transform.h"

namespace Kizuna {

    class TextureManager;

    class ModelManager {
    public:
        void Initialize(
            ID3D12Device *device,
            ID3D12GraphicsCommandList *commandList,
            TextureManager *textureManager);

        void CreateModel(
            const std::string &modelName,
            const TextureData &texture,
            uint32_t color = 0xFFFFFFFF,
            bool enableLighting = true);

        void CreateModel(
            const std::string &modelName,
            uint32_t color = 0xFFFFFFFF,
            bool enableLighting = true);

        void Update(
            CameraManager *camera);

        void Draw(
            ID3D12GraphicsCommandList *commandList);

        void DeleteModel(size_t index);

        std::vector<std::unique_ptr<Model>> &GetModels() {
            return models_;
        }

        std::vector<Transform> &GetTransforms() {
            return transforms_;
        }

        size_t GetModelCount() const {
            return models_.size();
        }

    private:
        ID3D12Device *device_ = nullptr;
        ID3D12GraphicsCommandList *commandList_ = nullptr;
        TextureManager *textureManager_ = nullptr;

        std::vector<std::unique_ptr<Model>> models_;
        std::vector<Transform> transforms_;
    };
}
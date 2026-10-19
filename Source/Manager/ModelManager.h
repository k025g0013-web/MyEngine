
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

        std::vector<std::unique_ptr<Model>> &GetModels() {
            return models_;
        }

        size_t GetModelCount() const {
            return models_.size();
        }

    private:
        ID3D12Device *device_ = nullptr;
        TextureManager *textureManager_ = nullptr;

        std::vector<std::unique_ptr<Model>> models_;
    };
}

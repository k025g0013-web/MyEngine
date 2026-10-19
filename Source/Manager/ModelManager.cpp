#include "ModelManager.h"

#include <cassert>

namespace Kizuna {

    void ModelManager::Initialize(
        ID3D12Device *device,
        TextureManager *textureManager) {

        assert(device);
        assert(textureManager);

        device_ = device;
        textureManager_ = textureManager;
    }

    void ModelManager::CreateModel(
        ID3D12GraphicsCommandList *commandList,
        const std::string &modelName,
        const TextureData &texture,
        uint32_t color,
        bool enableLighting) {

        assert(device_);
        assert(commandList);
        assert(textureManager_);

        auto model = std::make_unique<Model>(
            modelName,
            textureManager_,
            texture);

        model->Create(
            device_,
            commandList,
            color,
            enableLighting);

        models_.push_back(std::move(model));
    }

    void ModelManager::CreateModel(
        ID3D12GraphicsCommandList *commandList,
        const std::string &modelName,
        uint32_t color,
        bool enableLighting) {

        assert(device_);
        assert(commandList);
        assert(textureManager_);

        auto model = std::make_unique<Model>(
            modelName,
            textureManager_);

        model->Create(
            device_,
            commandList,
            color,
            enableLighting);

        models_.push_back(std::move(model));
    }

    void ModelManager::Update(
        CameraManager *camera) {

        for (auto &model : models_) {
            model->Update(camera);
        }
    }

    void ModelManager::Draw(
        ID3D12GraphicsCommandList *commandList) {

        assert(commandList);

        for (auto &model : models_) {
            model->Draw(commandList);
        }
    }

    void ModelManager::DeleteModel(size_t index) {

        assert(index < models_.size());

        models_.erase(models_.begin() + index);
    }
}
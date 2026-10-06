#include "ModelManager.h"

#include <cassert>

namespace Kizuna {

    void ModelManager::Initialize(
        ID3D12Device *device,
        ID3D12GraphicsCommandList *commandList,
        TextureManager *textureManager) {

        assert(device);
        assert(commandList);
        assert(textureManager);

        device_ = device;
        commandList_ = commandList;
        textureManager_ = textureManager;
    }

    void ModelManager::CreateModel(
        const std::string &modelName,
        const TextureData &texture,
        uint32_t color,
        bool enableLighting) {

        assert(device_);
        assert(commandList_);
        assert(textureManager_);

        auto model =
            std::make_unique<Model>(
                modelName,
                textureManager_,
                texture);

        model->Create(
            device_,
            commandList_,
            color,
            enableLighting);

        models_.push_back(
            std::move(model));

        transforms_.push_back({
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
            });
    }

    void ModelManager::CreateModel(
        const std::string &modelName,
        uint32_t color,
        bool enableLighting) {

        assert(device_);
        assert(commandList_);
        assert(textureManager_);

        auto model =
            std::make_unique<Model>(
                modelName,
                textureManager_);

        model->Create(
            device_,
            commandList_,
            color,
            enableLighting);

        models_.push_back(
            std::move(model));

        transforms_.push_back({
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
            });
    }

    void ModelManager::Update(
        CameraManager *camera) {

        for (size_t i = 0;
            i < models_.size();
            ++i) {

            models_[i]->Update(
                camera,
                transforms_[i]);
        }
    }

    void ModelManager::Draw(
        ID3D12GraphicsCommandList *commandList) {

        for (auto &model : models_) {

            model->Draw(
                commandList);
        }
    }

    void ModelManager::DeleteModel(
        size_t index) {

        assert(index < models_.size());
        assert(index < transforms_.size());

        models_.erase(
            models_.begin() + index);

        transforms_.erase(
            transforms_.begin() + index);
    }
}
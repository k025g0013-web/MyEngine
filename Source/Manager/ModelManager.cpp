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

        auto modelData = LoadModelResource(modelName);

        auto model = std::make_unique<Model>(
            modelName,
            textureManager_,
            texture,
            modelData);

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

        auto modelData = LoadModelResource(modelName);

        auto model = std::make_unique<Model>(
            modelName,
            textureManager_,
            modelData);

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

    void ModelManager::Finalize() {
        // 先にインスタンスを破棄
        models_.clear();

        // 次に共有リソースを解放
        modelResources_.clear();

        device_ = nullptr;
        textureManager_ = nullptr;
    }

    void ModelManager::DeleteModel(size_t index) {

        assert(index < models_.size());

        models_.erase(models_.begin() + index);
    }

    std::shared_ptr<ModelData> ModelManager::LoadModelResource(
        const std::string &modelName) {

        // 読み込み済みなら再利用
        auto it = modelResources_.find(modelName);

        if (it != modelResources_.end()) {
            return it->second;
        }

        // 初回だけOBJを読み込む
        auto modelData = std::make_shared<ModelData>(
            ModelLoader::LoadObjFile(modelName));

        // GPUメッシュも初回だけ生成
        modelData->mesh.Create(
            device_,
            modelData->vertices);

        // キャッシュに登録
        modelResources_.emplace(
            modelName,
            modelData);

        return modelData;
    }
}
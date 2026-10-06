#include "PrimitiveManager.h"

#include <cassert>

#include "Object/Primitive/TriangleObject.h"
#include "Object/Primitive/SphereObject.h"

namespace Kizuna {

    void PrimitiveManager::Initialize(
        ID3D12Device *device,
        ID3D12GraphicsCommandList *commandList) {

        assert(device);
        assert(commandList);

        device_ = device;
        commandList_ = commandList;
    }

    void PrimitiveManager::CreateTriangle(
        const Vector3 &left,
        const Vector3 &top,
        const Vector3 &right,
        const TextureData &texture,
        uint32_t color,
        bool enableLighting) {

        assert(device_);
        assert(commandList_);

        auto object =
            std::make_unique<TriangleObject>(
                left,
                top,
                right);

        object->SetTexture(texture);

        object->Create(
            device_,
            commandList_,
            color,
            enableLighting);

        objects_.push_back(
            std::move(object));

        transforms_.push_back({
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
            });
    }

    void PrimitiveManager::CreateSphere(
        int32_t subdivision,
        const TextureData &texture,
        uint32_t color,
        bool enableLighting) {

        assert(device_);
        assert(commandList_);

        auto object =
            std::make_unique<SphereObject>(
                subdivision);

        object->SetTexture(texture);

        object->Create(
            device_,
            commandList_,
            color,
            enableLighting);

        objects_.push_back(
            std::move(object));

        transforms_.push_back({
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
            });
    }

    void PrimitiveManager::Update(
        CameraManager *camera) {

        for (size_t i = 0;
            i < objects_.size();
            ++i) {

            objects_[i]->Update(
                camera,
                transforms_[i]);
        }
    }

    void PrimitiveManager::Draw(
        ID3D12GraphicsCommandList *commandList) {

        for (auto &object : objects_) {

            object->Draw(
                commandList);
        }
    }
}
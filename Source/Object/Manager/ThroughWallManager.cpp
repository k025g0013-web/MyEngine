#include "ThroughWallManager.h"

#include "Object/Common/Object3D.h"
#include "Utils/ColorHelper.h"

#include <algorithm>

namespace Kizuna {

    void ThroughWallManager::AddObject(
        Object3D *object,
        uint32_t color,
        Style style) {

        objects_.push_back(
            std::make_unique<ThroughWallObject>(
                object,
                UintToVector4(color),
                style
            )
        );
    }

    void ThroughWallManager::RemoveObject(Object3D *object) {

        objects_.erase(
            std::remove_if(
                objects_.begin(),
                objects_.end(),
                [object](const std::unique_ptr<ThroughWallObject> &data) {
                    return data->GetObject() == object;
                }
            ),
            objects_.end()
        );
    }

    void ThroughWallManager::Draw(
        ID3D12GraphicsCommandList *commandList) {

        for (auto &object : objects_) {
            object->Draw(commandList);
        }
    }
}
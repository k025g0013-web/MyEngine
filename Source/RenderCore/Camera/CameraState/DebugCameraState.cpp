#include "DebugCameraState.h"

#include "RenderCore/Camera/CameraManager.h"
#include "NormalCameraState.h"

namespace Kizuna {

    void DebugCameraState::Update(
        CameraManager &manager,
        const Transform &transform) {

        // 使用するカメラをデバッグカメラにする
        manager.SetCurrentCamera(manager.GetDebugCamera());

        // デバッグカメラを更新
        manager.GetDebugCamera()->Update(transform);
    }

    void DebugCameraState::Toggle(CameraManager &manager) {

        static NormalCameraState normalState;

        manager.ChangeState(&normalState);
    }
}
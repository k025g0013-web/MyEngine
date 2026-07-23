#include "NormalCameraState.h"

#include "RenderCore/Camera/CameraManager.h"
#include "DebugCameraState.h"

namespace Kizuna {

    void NormalCameraState::Update(
        CameraManager &manager,
        const Transform &transform) {

        // 使用するカメラを通常カメラにする
        manager.SetCurrentCamera(manager.GetNormalCamera());

        // 通常カメラを更新
        manager.GetNormalCamera()->Update(transform);
    }

    void NormalCameraState::Toggle(CameraManager &manager) {

        static DebugCameraState debugState;

        manager.ChangeState(&debugState);
    }
}
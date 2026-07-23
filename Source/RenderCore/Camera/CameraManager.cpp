#include "CameraManager.h"

#include "Math/FunctionVector.h"
#include "Math/FunctionMatrix.h"

#include <cassert>
#include <algorithm>
#include <cmath>

#include "CameraState/NormalCameraState.h"

namespace Kizuna {
    void CameraManager::Initialize(float width, float height, Keyboard *keyboard, Mouse *mouse, GamePad *gamePad) {
        // 通常カメラの初期化
        normalCamera_.SetScreenSize(width, height);

        // デバッグカメラの初期化
        debugCamera_.SetScreenSize(width, height);
        debugCamera_.SetInputDevice(keyboard, mouse, gamePad);

        static NormalCameraState normalState;

        state_ = &normalState;

        // 初期状態は通常カメラ
        currentCamera_ = &normalCamera_;
    }

    void CameraManager::Update(const Transform &transform) {
        // 現在使用中のカメラを更新
        state_->Update(*this, transform);
    }

    void CameraManager::ToggleCamera() {
        // 使用するカメラを切り替える
        state_->Toggle(*this);
    }
}
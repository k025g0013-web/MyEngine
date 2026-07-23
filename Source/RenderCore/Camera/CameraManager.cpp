#include "CameraManager.h"

#include "Math/FunctionVector.h"
#include "Math/FunctionMatrix.h"

#include <cassert>
#include <algorithm>
#include <cmath>

namespace Kizuna {
    void CameraManager::Initialize(float width, float height, Keyboard *keyboard, Mouse *mouse, GamePad *gamePad) {
        // 通常カメラの初期化
        normalCamera_.SetScreenSize(width, height);

        // デバッグカメラの初期化
        debugCamera_.SetScreenSize(width, height);
        debugCamera_.SetInputDevice(keyboard, mouse, gamePad);

        // 最初は通常カメラ
        currentCamera_ = &normalCamera_;
    }

    void CameraManager::Update(const Transform &transform) {
        // 現在使用中のカメラを更新
        currentCamera_->Update(transform);
    }

    void CameraManager::SetNormalCamera() { currentCamera_ = &normalCamera_; }

    void CameraManager::SetDebugCamera() { currentCamera_ = &debugCamera_; }

    void CameraManager::ToggleCamera() {
        if (currentCamera_ == &normalCamera_) {
            currentCamera_ = &debugCamera_;
        } else {
            currentCamera_ = &normalCamera_;
        }
    }
}
#include "CameraManager.h"

namespace Kizuna {

    void CameraManager::Initialize(
        float width,
        float height,
        InputManager *inputManager) {

        camera_.SetScreenSize(width, height);

        debugCamera_.SetScreenSize(width, height);
        debugCamera_.SetInputManager(inputManager);

        isDebugCamera_ = false;
    }

    void CameraManager::Update(const Transform &transform) {
        if (isDebugCamera_) {
            debugCamera_.Update();
        } else {
            camera_.Update(transform);
        }
    }

    void CameraManager::ToggleCamera() {
        isDebugCamera_ = !isDebugCamera_;
    }

    const Matrix4x4 &CameraManager::GetViewMatrix() const {
        if (isDebugCamera_) {
            return debugCamera_.GetViewMatrix();
        }
        return camera_.GetViewMatrix();
    }

    const Matrix4x4 &CameraManager::GetProjectionMatrix() const {
        if (isDebugCamera_) {
            return debugCamera_.GetProjectionMatrix();
        }
        return camera_.GetProjectionMatrix();
    }

    const Matrix4x4 &CameraManager::GetViewProjectionMatrix() const {
        if (isDebugCamera_) {
            return debugCamera_.GetViewProjectionMatrix();
        }
        return camera_.GetViewProjectionMatrix();
    }

}
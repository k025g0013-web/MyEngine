#pragma once

#include "Camera.h"
#include "Manager/InputManager.h"

namespace Kizuna {

    class DebugCamera {
    public:
        void SetScreenSize(float width, float height);
        void SetInputManager(InputManager *inputManager);

        void Update();

        const Matrix4x4 &GetViewMatrix() const {
            return camera_.GetViewMatrix();
        }

        const Matrix4x4 &GetProjectionMatrix() const {
            return camera_.GetProjectionMatrix();
        }

        const Matrix4x4 &GetViewProjectionMatrix() const {
            return camera_.GetViewProjectionMatrix();
        }

    private:
        void DebugMove();
        void DebugZoom();
        void DebugRotate();

    private:
        Camera camera_;
        InputManager *inputManager_ = nullptr;

        Vector3 debugTarget_{ 0.0f, 0.0f, 0.0f };
        Vector3 debugScreenOffset_{};
        Vector3 debugTranslation_{};
        Vector3 debugRotation_{};

        // 注視点からの距離
        float debugDistance_ = -5.0f;

        float debugMoveSpeed_ = 0.05f;
        float debugRotateSpeed_ = 0.005f;
    };

}
#pragma once

#include "RenderCore/Camera/Camera.h"
#include "RenderCore/Camera/DebugCamera.h"
#include "Manager/InputManager.h"

namespace Kizuna {

    class CameraManager {
    public:
        void Initialize(
            float width,
            float height,
            InputManager *inputManager);

        void Update(const Transform &transform);
        void ToggleCamera();

        bool IsDebugCamera() const {
            return isDebugCamera_;
        }

        const char *GetStateName() const {
            return isDebugCamera_ ? "DEBUG" : "NORMAL";
        }

        const Matrix4x4 &GetViewMatrix() const;
        const Matrix4x4 &GetProjectionMatrix() const;
        const Matrix4x4 &GetViewProjectionMatrix() const;

    private:
        Camera camera_;
        DebugCamera debugCamera_;

        bool isDebugCamera_ = false;
    };

}
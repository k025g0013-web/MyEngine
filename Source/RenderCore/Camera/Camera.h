#pragma once

#include "Math/Matrix.h"
#include "Math/Transform.h"

namespace Kizuna {
    class Camera {
    public:
        void SetScreenSize(float width, float height);

        // 通常カメラの行列を更新する
        void Update(const Transform &transform);

        const Matrix4x4 &GetViewMatrix() const {
            return viewMatrix_;
        }

        const Matrix4x4 &GetProjectionMatrix() const {
            return projectionMatrix_;
        }

        const Matrix4x4 &GetViewProjectionMatrix() const {
            return viewProjectionMatrix_;
        }

    private:
        void UpdateMatrices(const Transform &transform);

    private:
        Matrix4x4 viewMatrix_{};
        Matrix4x4 projectionMatrix_{};
        Matrix4x4 viewProjectionMatrix_{};

        float width_ = 1280.0f;
        float height_ = 720.0f;

        float fovY_ = 0.45f;
        float nearClip_ = 0.1f;
        float farClip_ = 100.0f;
    };
}
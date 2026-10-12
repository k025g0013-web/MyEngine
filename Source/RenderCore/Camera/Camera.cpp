#include "Camera.h"

#include "Math/FunctionVector.h"
#include "Math/FunctionMatrix.h"

#include <cassert>

namespace Kizuna {
    void Camera::SetScreenSize(float width, float height) {
        assert(width > 0.0f);
        assert(height > 0.0f);

        width_ = width;
        height_ = height;
    }

    void Camera::Update(const Transform &transform) {
        UpdateMatrices(transform);
    }

    void Camera::UpdateMatrices(const Transform &transform) {
        Matrix4x4 cameraMatrix = Math::MakeAffineMatrix(
            transform.scale,
            transform.rotate,
            transform.translate);

        viewMatrix_ = Math::Inverse(cameraMatrix);

        projectionMatrix_ = Math::MakePerspectiveFovMatrix(
            fovY_,
            width_ / height_,
            nearClip_,
            farClip_);

        viewProjectionMatrix_ =
            Math::Multiply(viewMatrix_, projectionMatrix_);
    }

}
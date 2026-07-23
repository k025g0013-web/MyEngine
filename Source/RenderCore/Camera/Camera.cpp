#include "Camera.h"

#include "Math/FunctionVector.h"
#include "Math/FunctionMatrix.h"

#include <cassert>
#include <algorithm>
#include <cmath>

namespace Kizuna {
    void Camera::SetScreenSize(float width, float height) {
        // 描画に使用する画面サイズを保持する
        width_ = width;
        height_ = height;
    }

    void Camera::Update(const Transform &transform) {
        // カメラに使うMatrix群の更新
        UpdateMatrices(transform);
    }

    void Camera::UpdateMatrices(const Transform &transform) {
        // Transformからカメラ行列を生成する
        Matrix4x4 cameraMatrix = Math::MakeAffineMatrix(
            transform.scale, transform.rotate, transform.translate);

        // カメラ行列の逆行列をビュー行列として使用する
        viewMatrix_ = Math::Inverse(cameraMatrix);

        // 透視投影行列を生成する
        projectionMatrix_ = Math::MakePerspectiveFovMatrix(
            fovY_, width_ / height_, nearClip_, farClip_);

        // 描画で使用するViewProjection行列を生成する
        viewProjectionMatrix_ = Math::Multiply(viewMatrix_, projectionMatrix_);
    }
}
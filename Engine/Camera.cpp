#include "Camera.h"

#include "MathFunctions.h"

void Camera::Initialize(float width, float height) {
    width_ = width;
    height_ = height;
}

void Camera::Update(Transform &transform) {
    Matrix4x4 cameraMatrix = MakeWorldMatrix(transform);

    viewMatrix_ = Inverse(cameraMatrix);

    projectionMatrix_ = MakePerspectiveFovMatrix(
        fovY_, width_ / height_, nearClip_, farClip_
    );

    viewProjectionMatrix_ =
        Multiply(viewMatrix_, projectionMatrix_);
}
#include "Camera.h"

#include "MathFunctions.h"

void Camera::Initialize(float width, float height) {

    width_ = width;
    height_ = height;

    transform_.scale = { 1.0f,1.0f,1.0f };
    transform_.rotate = { 0.3f,0.0f,0.0f };
    transform_.translate = { 0.0f,4.0f,-10.0f };
}

void Camera::Update() {
    Matrix4x4 cameraMatrix = MakeWorldMatrix(transform_);

    viewMatrix_ = Inverse(cameraMatrix);

    projectionMatrix_ = MakePerspectiveFovMatrix(
        fovY_, width_ / height_, nearClip_, farClip_
    );

    viewProjectionMatrix_ =
        Multiply(viewMatrix_, projectionMatrix_);
}
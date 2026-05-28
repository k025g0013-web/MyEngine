#pragma once

#include "Transform.h"
#include "Matrix4x4.h"

class Camera {
public:
    void Initialize(float width, float height);

    void Update();

    // getter
    const Matrix4x4 &GetViewMatrix() const {return viewMatrix_;}
    const Matrix4x4 &GetProjectionMatrix() const {return projectionMatrix_;}
    const Matrix4x4 &GetViewProjectionMatrix() const {return viewProjectionMatrix_;}
    Transform &GetTransform() {return transform_;}

private:
    Transform transform_{
         {1.0f,1.0f,1.0f},
         {0.0f,0.0f,0.0f},
         {0.0f,0.0f,-10.0f}
    };

    Matrix4x4 viewMatrix_;
    Matrix4x4 projectionMatrix_;
    Matrix4x4 viewProjectionMatrix_;

    float width_ = 1280.0f;
    float height_ = 720.0f;

    float fovY_ = 0.45f;
    float nearClip_ = 0.1f;
    float farClip_ = 100.0f;
};
#pragma once

#include "InputKey.h"
#include "InputMouse.h"

#include "Vector3.h"
#include "Matrix4x4.h"

class DebugCamera {
public:
	void Initialize(InputKey *keyboard, InputMouse *mouse);
	void Update();

    void CameraMove();
    void CameraZoom();
    void CameraRotate();

    // getter
    const Vector3 &GetTarget() const { return target_; }
    const Vector3 &GetTranslation() const { return translation_; }
    const Vector3 &GetRotation() const { return rotation_; }

    const Matrix4x4 &GetViewMatrix() const { return viewMatrix_; }
    const Matrix4x4 &GetProjectionMatrix() const { return projectionMatrix_; }
    const Matrix4x4 &GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

private:
    Vector3 target_ = { 0.0f, 0.0f, 0.0f };
    
    // カメラ平行移動
    InputKey* keyboard_{};
    float moveSpeed_ = 0.05f;
    
    // マウス操作
    InputMouse* mouse_{};
    
    // カメラズーム 
    float distance_ = -5.0f;

    // カメラ回転
    float rotateSpeed_ = 0.005f;

    // 計算結果
    Vector3 screenOffset_{};
    
    Vector3 translation_{};
    Vector3 rotation_{};

    Matrix4x4 viewMatrix_{};
    Matrix4x4 projectionMatrix_{};
    Matrix4x4 viewProjectionMatrix_{};

    float width_ = 1280.0f;
    float height_ = 720.0f;

    float fovY_ = 0.45f;
    float nearClip_ = 0.1f;
    float farClip_ = 100.0f;

    POINT dragStart_{};
    bool isDragging_ = false;
};
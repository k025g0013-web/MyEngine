#pragma once

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Transform.h"

class Camera {
public:
    // カメラのモード定義
    enum class Mode {
        Normal, // 通常
        Debug,  // デバッグ用
    };

public:
    void Initialize(float width, float height, Keyboard *keyboard, Mouse *mouse, GamePad *gamePad);
    void Update(const Transform &normalTargetTransform);

    // モード切り替え
    void SetMode(Mode mode) { mode_ = mode; }
    Mode GetMode() const { return mode_; }

    // モードを交互に切り替え
    void ToggleMode();

    // getter
    const Matrix4x4 &GetViewMatrix() const {return viewMatrix_;}
    const Matrix4x4 &GetProjectionMatrix() const {return projectionMatrix_;}
    const Matrix4x4 &GetViewProjectionMatrix() const {return viewProjectionMatrix_;}
   
    // デバッグ用getter
    const Vector3 &GetDebugTarget() const { return debugTarget_; }
    const Vector3 &GetDebugRotation() const { return debugRotation_; }

    const float &GetDebugDistance() const { return debugDistance_; }

private:
    // 内部更新
    void UpdateCamera(const Transform &targetTransform);
    void UpdateDebug();

    // デバッグ用操作
    void DebugMove();
    void DebugZoom();
    void DebugRotate();

private:
    // 現在のモード
    Mode mode_ = Mode::Normal;

    // 計算結果
    Matrix4x4 viewMatrix_{};
    Matrix4x4 projectionMatrix_{};
    Matrix4x4 viewProjectionMatrix_{};
    
    // 画面設定
    float width_ = 1280.0f;
    float height_ = 720.0f;
    float fovY_ = 0.45f;
    float nearClip_ = 0.1f;
    float farClip_ = 100.0f;

    // 入力デバイス
    Keyboard *keyboard_ = nullptr;
    Mouse *mouse_ = nullptr;
    GamePad *gamePad_ = nullptr;

    // カメラ中央軸
    Vector3 debugTarget_ = { 0.0f, 0.0f, 0.0f };
    Vector3 debugScreenOffset_{};
    Vector3 debugTranslation_{};
    Vector3 debugRotation_{};

    // デバッグ操作感度
    float debugDistance_ = -5.0f;
    float debugMoveSpeed_ = 0.05f;
    float debugRotateSpeed_ = 0.005f;
};
#include "Camera.h"

#include "Math/FunctionVector.h"
#include "Math/FunctionMatrix.h"

#include <cassert>
#include <algorithm>
#include <cmath>

void Camera::Initialize(float width, float height, Keyboard *keyboard, Mouse *mouse, GamePad *gamePad) {
    width_ = width;
    height_ = height;

    keyboard_ = keyboard;
    mouse_ = mouse;
    gamePad_ = gamePad;
}

void Camera::Update(const Transform &transform) {
    // モードによって処理を分ける
    switch (mode_) {
    case Mode::Normal:
        UpdateCamera(transform);
        break;

    case Mode::Debug:
        UpdateDebug();
        break;
    }

    // どちらのモードであっても、最終的に共通のViewProjection行列を合成する
    viewProjectionMatrix_ = Math::Multiply(viewMatrix_, projectionMatrix_);
}

// モードを交互に切り替え
void Camera::ToggleMode() {
    if (mode_ == Mode::Normal) {
        mode_ = Mode::Debug;
    } else {
        mode_ = Mode::Normal;
    }
}

// --- 通常カメラの更新 ---
void Camera::UpdateCamera(const Transform &targetTransform) {
    Matrix4x4 cameraMatrix = Math::MakeAffineMatrix(targetTransform.scale, targetTransform.rotate, targetTransform.translate);
    viewMatrix_ = Math::Inverse(cameraMatrix);

    projectionMatrix_ = Math::MakePerspectiveFovMatrix(
        fovY_, width_ / height_, nearClip_, farClip_
    );
}

// --- デバッグカメラの更新 ---
void Camera::UpdateDebug() {
    assert(keyboard_ && "キーボードを検出できませんでした");
    assert(mouse_ && "マウスを検出できませんでした");
    if (gamePad_ && gamePad_->IsConnected()) {
        if (gamePad_->TriggerButton(XINPUT_GAMEPAD_BACK)) {
            ToggleMode();
            return;
        }
    }

    // キー・マウス入力による座標更新
    DebugMove();
    DebugZoom();
    DebugRotate();

    // 以前のDebugCamera.cppの計算ロジック
    Vector3 right = {
        sinf(debugRotation_.y - float(M_PI) / 2.0f),
        0.0f,
        cosf(debugRotation_.y - float(M_PI) / 2.0f)
    };
    Vector3 up = { 0, 1, 0 };

    Vector3 offsetTarget{
        debugTarget_.x + right.x * debugScreenOffset_.x + up.x * debugScreenOffset_.y,
        debugTarget_.y + right.y * debugScreenOffset_.x + up.y * debugScreenOffset_.y,
        debugTarget_.z + right.z * debugScreenOffset_.x + up.z * debugScreenOffset_.y,
    };

    Vector3 forwardXZ = { sinf(debugRotation_.y), 0.0f, cosf(debugRotation_.y) };
    Vector3 forward = {
        forwardXZ.x * cosf(debugRotation_.x),
        -sinf(debugRotation_.x),
        forwardXZ.z * cosf(debugRotation_.x)
    };

    debugTranslation_ = {
        offsetTarget.x + forward.x * debugDistance_,
        offsetTarget.y + forward.y * debugDistance_,
        offsetTarget.z + forward.z * debugDistance_,
    };

    Transform transform{};
    transform.scale = { 1, 1, 1 };
    transform.rotate = debugRotation_;
    transform.translate = debugTranslation_;

    UpdateCamera(transform);
}

// デバッグ用操作
void Camera::DebugMove() {
    // キーボード入力
    if (keyboard_->PushKey(DIK_W)) debugScreenOffset_.y += debugMoveSpeed_;
    if (keyboard_->PushKey(DIK_S)) debugScreenOffset_.y -= debugMoveSpeed_;
    if (keyboard_->PushKey(DIK_D)) debugScreenOffset_.x += debugMoveSpeed_;
    if (keyboard_->PushKey(DIK_A)) debugScreenOffset_.x -= debugMoveSpeed_;

    // ゲームパッド入力
    if (gamePad_ && gamePad_->IsConnected()) {
        float stickX = 0.0f;
        float stickY = 0.0f;
        gamePad_->GetLeftStick(stickX, stickY);

        // スティックの傾きに応じて移動
        debugScreenOffset_.x += stickX * debugMoveSpeed_;
        debugScreenOffset_.y += stickY * debugMoveSpeed_;

        // 十字キーでも動かせるようにする場合
        if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_UP))    debugScreenOffset_.y += debugMoveSpeed_;
        if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_DOWN))  debugScreenOffset_.y -= debugMoveSpeed_;
        if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT)) debugScreenOffset_.x += debugMoveSpeed_;
        if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_LEFT))  debugScreenOffset_.x -= debugMoveSpeed_;
    }
}

void Camera::DebugZoom() {
    // マウスホイール入力
    int wheel = mouse_->GetWheelDelta();
    if (wheel != 0) {
        debugDistance_ -= static_cast<float>(wheel) * 0.0005f;
    }

    // ゲームパッド入力
    if (gamePad_ && gamePad_->IsConnected()) {
        float leftTrigger = gamePad_->GetLeftTrigger();
        float rightTrigger = gamePad_->GetRightTrigger();

        // 右トリガーでズームイン、左トリガーでズームアウト（速度は 0.1f 等で調整）
        debugDistance_ += rightTrigger * 0.1f;
        debugDistance_ -= leftTrigger * 0.1f;
    }

    // クランプ処理
    const float minDistance = -500.0f;
    const float maxDistance = -1.0f;
    debugDistance_ = std::clamp(debugDistance_, minDistance, maxDistance);
}

void Camera::DebugRotate() {
    // マウス入力
    if (mouse_->PushLeft()) {
        debugRotation_.y += mouse_->GetDeltaX() * debugRotateSpeed_;
        debugRotation_.x += mouse_->GetDeltaY() * debugRotateSpeed_;
    }

    // ゲームパッド入力
    if (gamePad_ && gamePad_->IsConnected()) {
        float stickX = 0.0f;
        float stickY = 0.0f;
        gamePad_->GetRightStick(stickX, stickY);

        // 右スティックの入力を回転角に加算
        debugRotation_.y += stickX * (debugRotateSpeed_ * 4.0f);
        debugRotation_.x += stickY * (debugRotateSpeed_ * 4.0f);
    }

    // クランプ処理（最後に共通で行う）
    const float limit = float(M_PI_2) - 0.01f;
    debugRotation_.x = std::clamp(debugRotation_.x, -limit, limit);
}
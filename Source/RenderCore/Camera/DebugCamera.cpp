#include "DebugCamera.h"

#include "Math/FunctionVector.h"
#include "Math/FunctionMatrix.h"

#include <cassert>
#include <algorithm>
#include <cmath>

namespace Kizuna {
    void DebugCamera::SetInputDevice(
        Keyboard *keyboard, Mouse *mouse, GamePad *gamePad) {
        // デバッグ操作で利用する入力デバイスを保持する
        keyboard_ = keyboard;
        mouse_ = mouse;
        gamePad_ = gamePad;
    }

    void DebugCamera::Update(const Transform &) {
        // 入力デバイスが存在しない場合はデバッグ操作できないため停止する
        assert(keyboard_ && "キーボードを検出できませんでした");
        assert(mouse_ && "マウスを検出できませんでした");

        // 入力に応じて注視点・回転・ズーム量を更新する
        DebugMove();
        DebugZoom();
        DebugRotate();

        // カメラの右方向ベクトルを求める
        Vector3 right = {sinf(debugRotation_.y - float(M_PI) / 2.0f), 0.0f, cosf(debugRotation_.y - float(M_PI) / 2.0f)};

        // ワールド座標系の上方向ベクトル
        Vector3 up = {0.0f, 1.0f, 0.0f};

        // 画面移動量をワールド座標へ変換する
        Vector3 offsetTarget = debugTarget_ + right * debugScreenOffset_.x + up * debugScreenOffset_.y;

        // カメラの前方向ベクトルを算出する
        Vector3 forwardXZ = { sinf(debugRotation_.y), 0.0f, cosf(debugRotation_.y) };
        Vector3 forward = {forwardXZ.x * cosf(debugRotation_.x), -sinf(debugRotation_.x), forwardXZ.z * cosf(debugRotation_.x)};

        // 注視点から距離分だけ離れた位置をカメラ座標とする
        debugTranslation_ = offsetTarget + forward * debugDistance_;

        // デバッグカメラ用Transformを作成する
        Transform transform{};
        transform.scale = {1.0f, 1.0f, 1.0f};
        transform.rotate = debugRotation_;
        transform.translate = debugTranslation_;

        // カメラに使うMatrix群の更新
        UpdateMatrices(transform);
    }

    void DebugCamera::DebugMove() {
        // WASDキーで注視点を平行移動する
        if (keyboard_->PushKey(DIK_W)) debugScreenOffset_.y += debugMoveSpeed_;
        if (keyboard_->PushKey(DIK_S)) debugScreenOffset_.y -= debugMoveSpeed_;
        if (keyboard_->PushKey(DIK_D)) debugScreenOffset_.x += debugMoveSpeed_;
        if (keyboard_->PushKey(DIK_A)) debugScreenOffset_.x -= debugMoveSpeed_;

        // 左スティック入力による視点移動
        if (gamePad_ && gamePad_->IsConnected()) {
            float stickX = 0.0f;
            float stickY = 0.0f;
            gamePad_->GetLeftStick(stickX, stickY);

            // 左スティックでも平行移動できるようにする
            debugScreenOffset_.x += stickX * debugMoveSpeed_;
            debugScreenOffset_.y += stickY * debugMoveSpeed_;

            // 十字キーでも操作できるようにする
            if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_UP))    debugScreenOffset_.y += debugMoveSpeed_;
            if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_DOWN))  debugScreenOffset_.y -= debugMoveSpeed_;
            if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT)) debugScreenOffset_.x += debugMoveSpeed_;
            if (gamePad_->PushButton(XINPUT_GAMEPAD_DPAD_LEFT))  debugScreenOffset_.x -= debugMoveSpeed_;
        }
    }

    void DebugCamera::DebugZoom() {
        // マウスホイールでカメラ距離を変更する
        int wheel = mouse_->GetWheelDelta();
        if (wheel != 0) {
            debugDistance_ -= static_cast<float>(wheel) * 0.0005f;
        }

        // トリガー入力でズームイン・ズームアウトする
        if (gamePad_ && gamePad_->IsConnected()) {
            float leftTrigger = gamePad_->GetLeftTrigger();
            float rightTrigger = gamePad_->GetRightTrigger();

            // トリガーの入力量に応じてズーム量を加算する
            debugDistance_ += rightTrigger * 0.1f;
            debugDistance_ -= leftTrigger * 0.1f;
        }

        // カメラが注視点を通り越さないよう距離を制限する
        const float minDistance = -500.0f;
        const float maxDistance = -1.0f;
        debugDistance_ = std::clamp(debugDistance_, minDistance, maxDistance);
    }

    void DebugCamera::DebugRotate() {
        // マウスドラッグで視点を回転させる
        if (mouse_->PushLeft()) {
            debugRotation_.y += mouse_->GetDeltaX() * debugRotateSpeed_;
            debugRotation_.x += mouse_->GetDeltaY() * debugRotateSpeed_;
        }

        // 右スティック入力による視点回転
        if (gamePad_ && gamePad_->IsConnected()) {
            float stickX = 0.0f;
            float stickY = 0.0f;
            gamePad_->GetRightStick(stickX, stickY);

            // 右スティックでも視点を回転できるようにする
            debugRotation_.y += stickX * (debugRotateSpeed_ * 4.0f);
            debugRotation_.x += stickY * (debugRotateSpeed_ * 4.0f);
        }

        // 真上・真下を向いてジンバルロックに近い状態になることを防ぐ
        const float limit = float(M_PI_2) - 0.01f;
        debugRotation_.x = std::clamp(debugRotation_.x, -limit, limit);
    }
}
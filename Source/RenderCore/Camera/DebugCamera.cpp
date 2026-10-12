#include "DebugCamera.h"

#include <algorithm>
#include <cassert>
#include <numbers>
#include <cmath>

namespace Kizuna {

    void DebugCamera::SetScreenSize(float width, float height) {
        camera_.SetScreenSize(width, height);
    }

    void DebugCamera::SetInputManager(InputManager *inputManager) {
        inputManager_ = inputManager;
    }

    void DebugCamera::Update() {
        assert(inputManager_ != nullptr);

        if (!inputManager_) {
            return;
        }

        DebugMove();
        DebugZoom();
        DebugRotate();

        // カメラの右方向
        Vector3 right = {
            std::sin(debugRotation_.y - std::numbers::pi_v<float> / 2.0f),
            0.0f,
            std::cos(debugRotation_.y - std::numbers::pi_v<float> / 2.0f)
        };

        // ワールド座標系の上方向
        Vector3 up = { 0.0f, 1.0f, 0.0f };

        // 注視点の平行移動
        Vector3 offsetTarget =
            debugTarget_
            + right * debugScreenOffset_.x
            + up * debugScreenOffset_.y;

        // カメラの前方向
        Vector3 forwardXZ = {
            std::sin(debugRotation_.y),
            0.0f,
            std::cos(debugRotation_.y)
        };

        Vector3 forward = {
            forwardXZ.x * std::cos(debugRotation_.x),
            -std::sin(debugRotation_.x),
            forwardXZ.z * std::cos(debugRotation_.x)
        };

        // 注視点から距離分だけ離れた位置
        debugTranslation_ = offsetTarget + forward * debugDistance_;

        Transform transform{};
        transform.scale = { 1.0f, 1.0f, 1.0f };
        transform.rotate = debugRotation_;
        transform.translate = debugTranslation_;

        camera_.Update(transform);
    }

    void DebugCamera::DebugMove() {
        // キーボードによる平行移動
        if (inputManager_->PushKey(DIK_W)) {
            debugScreenOffset_.y += debugMoveSpeed_;
        }
        if (inputManager_->PushKey(DIK_S)) {
            debugScreenOffset_.y -= debugMoveSpeed_;
        }
        if (inputManager_->PushKey(DIK_D)) {
            debugScreenOffset_.x += debugMoveSpeed_;
        }
        if (inputManager_->PushKey(DIK_A)) {
            debugScreenOffset_.x -= debugMoveSpeed_;
        }

        // ゲームパッドによる平行移動
        if (inputManager_->IsConnected()) {
            float stickX = 0.0f;
            float stickY = 0.0f;
            inputManager_->GetLeftStick(stickX, stickY);

            debugScreenOffset_.x += stickX * debugMoveSpeed_;
            debugScreenOffset_.y += stickY * debugMoveSpeed_;

            if (inputManager_->PushButton(XINPUT_GAMEPAD_DPAD_UP)) {
                debugScreenOffset_.y += debugMoveSpeed_;
            }
            if (inputManager_->PushButton(XINPUT_GAMEPAD_DPAD_DOWN)) {
                debugScreenOffset_.y -= debugMoveSpeed_;
            }
            if (inputManager_->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT)) {
                debugScreenOffset_.x += debugMoveSpeed_;
            }
            if (inputManager_->PushButton(XINPUT_GAMEPAD_DPAD_LEFT)) {
                debugScreenOffset_.x -= debugMoveSpeed_;
            }
        }
    }

    void DebugCamera::DebugZoom() {
        // マウスホイールによるズーム
        int wheel = inputManager_->GetWheelDelta();
        if (wheel != 0) {
            debugDistance_ -= static_cast<float>(wheel) * 0.0005f;
        }

        // ゲームパッドのトリガーによるズーム
        if (inputManager_->IsConnected()) {
            float leftTrigger = inputManager_->GetLeftTrigger();
            float rightTrigger = inputManager_->GetRightTrigger();

            debugDistance_ += rightTrigger * 0.1f;
            debugDistance_ -= leftTrigger * 0.1f;
        }

        debugDistance_ = std::clamp(debugDistance_, -500.0f, -1.0f);
    }

    void DebugCamera::DebugRotate() {
        // マウスドラッグによる回転
        if (inputManager_->PushMouseLeft()) {
            debugRotation_.y +=
                static_cast<float>(inputManager_->GetDeltaX())
                * debugRotateSpeed_;

            debugRotation_.x +=
                static_cast<float>(inputManager_->GetDeltaY())
                * debugRotateSpeed_;
        }

        // ゲームパッドの右スティックによる回転
        if (inputManager_->IsConnected()) {
            float stickX = 0.0f;
            float stickY = 0.0f;
            inputManager_->GetRightStick(stickX, stickY);

            debugRotation_.y += stickX * (debugRotateSpeed_ * 4.0f);
            debugRotation_.x += stickY * (debugRotateSpeed_ * 4.0f);
        }

        // 真上・真下を向きすぎないように制限
        const float limit =
            std::numbers::pi_v<float> / 2.0f - 0.01f;

        debugRotation_.x =
            std::clamp(debugRotation_.x, -limit, limit);
    }

}